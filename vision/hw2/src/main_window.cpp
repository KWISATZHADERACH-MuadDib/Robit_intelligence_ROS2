#include "main_window.hpp"
#include "ui_main_window.h"   // AUTOUIC이 main_window.ui로부터 빌드 시 자동 생성하는 헤더

#include <QSlider>

#include "feature_extractor.hpp"
#include "qt_image.hpp"
#include "result_renderer.hpp"

// 생성자: QMainWindow 초기화 + rclcpp::Node 초기화 + UI 세팅 + 구독자/타이머 설정
MainWindowDesign::MainWindowDesign(QWidget * parent)
: QMainWindow(parent),
  rclcpp::Node("gui_node"),   // ROS2 노드 이름 지정
  ui_(new Ui::MainWindowDesign)
{
  ui_->setupUi(this);   // .ui 파일에 정의된 위젯들을 이 윈도우에 배치

  // 이미지 라벨은 pixmap 크기 때문에 레이아웃이 커지지 않도록 Ignored 정책 적용
  for (QLabel * label : {
      ui_->USB_screen, ui_->Object_screen, ui_->White_line_screen, ui_->Blue_line_screen,
      ui_->Neon_cone_screen, ui_->Orange_cone_screen, ui_->Neon_crop, ui_->Orange_crop})
  {
    label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    label->setMinimumSize(1, 1);
  }

  // 슬라이더 범위: H는 0~179, S/V는 0~255
  for (QSlider * s : {ui_->Hue_low_slider, ui_->Hue_high_slider}) {
    s->setRange(0, 179);
  }
  for (QSlider * s : {
      ui_->Sat_low_slider, ui_->Sat_high_slider, ui_->Value_low_slider, ui_->Value_high_slider})
  {
    s->setRange(0, 255);
  }

  // 라디오 버튼: 조절 대상 선택
  for (QRadioButton * b : { ui_->White_line_Button, ui_->Blue_line_Button, ui_->Neon_cone_Button, ui_->Orange_cone_Button})
  {
    connect(b, &QRadioButton::toggled, this, &MainWindowDesign::onObjectSelected);
  }

  // 슬라이더: 값이 바뀌면 선택된 대상의 범위 갱신
  for (QSlider * s : {
      ui_->Hue_low_slider, ui_->Hue_high_slider, ui_->Sat_low_slider, ui_->Sat_high_slider,
      ui_->Value_low_slider, ui_->Value_high_slider})
  {
    connect(s, &QSlider::valueChanged, this, &MainWindowDesign::onSliderChanged);
  }

  ui_->White_line_Button->setChecked(true);   // toggled -> onObjectSelected로 슬라이더 초기화

  // "/image_raw" 토픽 구독 (큐 크기 10)
  image_sub_ = create_subscription<sensor_msgs::msg::Image>(
    "/image_raw", 10,
    std::bind(&MainWindowDesign::imageCallback, this, std::placeholders::_1));

  // 30ms마다 spinRos() 호출 → 약 33fps로 ROS2 콜백 처리
  ros_timer_ = new QTimer(this);
  connect(ros_timer_, &QTimer::timeout, this, &MainWindowDesign::spinRos);
  ros_timer_->start(30);
}

MainWindowDesign::~MainWindowDesign()
{
  delete ui_;
}

// QTimer가 주기적으로 호출: 큐에 쌓인 ROS2 콜백들을 처리
void MainWindowDesign::spinRos()
{
  rclcpp::spin_some(this->get_node_base_interface());
}

vision::ObjectId MainWindowDesign::selectedObject() const
{
  if (ui_->Blue_line_Button->isChecked()) {
    return vision::BLUE_LINE;
  }
  if (ui_->Neon_cone_Button->isChecked()) {
    return vision::NEON_CONE;
  }
  if (ui_->Orange_cone_Button->isChecked()) {
    return vision::ORANGE_CONE;
  }
  return vision::WHITE_LINE;
}

vision::HsvRange MainWindowDesign::rangeFromSliders() const
{
  return {
    ui_->Hue_low_slider->value(), ui_->Hue_high_slider->value(),
    ui_->Sat_low_slider->value(), ui_->Sat_high_slider->value(),
    ui_->Value_low_slider->value(), ui_->Value_high_slider->value()};
}

void MainWindowDesign::loadSlidersFromRange(const vision::HsvRange & r)
{
  struct Item { QSlider * slider; QLabel * value_label; int value; };
  const Item items[] = {
    {ui_->Hue_low_slider, ui_->Hue_low_value, r.h_low},
    {ui_->Hue_high_slider, ui_->Hue_high_value, r.h_high},
    {ui_->Sat_low_slider, ui_->Sat_low_value, r.s_low},
    {ui_->Sat_high_slider, ui_->Sat_high_value, r.s_high},
    {ui_->Value_low_slider, ui_->Val_low_value, r.v_low},
    {ui_->Value_high_slider, ui_->Val_high_value, r.v_high},
  };
  for (const Item & it : items) {
    // 값 세팅 중에는 valueChanged가 범위를 덮어쓰지 않도록 시그널 차단
    const QSignalBlocker blocker(it.slider);
    it.slider->setValue(it.value);
    it.value_label->setText(QString::number(it.value));
  }
}

// 라디오 버튼 변경: 해당 대상의 저장된 범위를 슬라이더에 반영
void MainWindowDesign::onObjectSelected()
{
  loadSlidersFromRange(detector_.range(selectedObject()));
}

// 슬라이더 변경: 선택된 대상의 범위를 갱신하고 화면을 즉시 다시 그림
void MainWindowDesign::onSliderChanged()
{
  const vision::HsvRange r = rangeFromSliders();
  detector_.setRange(selectedObject(), r);

  ui_->Hue_low_value->setText(QString::number(r.h_low));
  ui_->Hue_high_value->setText(QString::number(r.h_high));
  ui_->Sat_low_value->setText(QString::number(r.s_low));
  ui_->Sat_high_value->setText(QString::number(r.s_high));
  ui_->Val_low_value->setText(QString::number(r.v_low));
  ui_->Val_high_value->setText(QString::number(r.v_high));

  updateDisplay();   // 카메라가 멈춰 있어도 마스크가 바로 바뀌도록
}

// 카메라 이미지가 도착할 때마다 호출되는 콜백
void MainWindowDesign::imageCallback(const sensor_msgs::msg::Image::SharedPtr msg)
{
  cv_bridge::CvImagePtr cv_ptr;
  try {
    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
  } catch (cv_bridge::Exception & e) {
    RCLCPP_ERROR(get_logger(), "cv_bridge error: %s", e.what());
    return;
  }

  latest_frame_ = cv_ptr->image;
  updateDisplay();
}

void MainWindowDesign::updateDisplay()
{
  if (latest_frame_.empty()) {
    return;
  }

  const vision::DetectionResult result = detector_.detect(latest_frame_);

  showMatOnLabel(latest_frame_, ui_->USB_screen);
  showMatOnLabel(vision::renderResult(latest_frame_, result), ui_->Object_screen);
  showMatOnLabel(result.masks[vision::WHITE_LINE], ui_->White_line_screen);
  showMatOnLabel(result.masks[vision::BLUE_LINE], ui_->Blue_line_screen);
  showMatOnLabel(result.masks[vision::NEON_CONE], ui_->Neon_cone_screen);
  showMatOnLabel(result.masks[vision::ORANGE_CONE], ui_->Orange_cone_screen);

  // 바운딩박스 내부만 확대해서 표시 (콘이 없으면 None)
  const auto show_crop = [this](const vision::ConeInfo & cone, QLabel * label) {
      if (!cone.found) {
        label->clear();
        label->setText("None");
        return;
      }
      showMatOnLabel(vision::cropCone(latest_frame_, cone), label);
    };
  show_crop(result.neon_cone, ui_->Neon_crop);
  show_crop(result.orange_cone, ui_->Orange_crop);

  // 콘 위치 텍스트 (콘이 없으면 none)
  const auto show_position = [](const vision::ConePosition & p, QLabel * v, QLabel * h) {
      v->setText(p.found ? vision::toString(p.vertical) : "none");
      h->setText(p.found ? vision::toString(p.horizontal) : "none");
    };
  show_position(result.neon_position, ui_->Neon_vertical, ui_->Neon_horizontal);
  show_position(result.orange_position, ui_->Orange_vertical, ui_->Orange_horizontal);
}
