#ifndef MAIN_WINDOW_HPP_
#define MAIN_WINDOW_HPP_

#include <QMainWindow>
#include <QTimer>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/core.hpp>

#include "cone_detector.hpp"

// .ui 파일에서 AUTOUIC이 자동 생성하는 클래스 (실제 정의는 cpp에서 include)
namespace Ui
{
class MainWindowDesign;
}

// QMainWindow(화면 담당) + rclcpp::Node(ROS2 노드 담당)를 동시에 상속.
// 영상처리는 vision::ConeDetector에 맡기고, 여기서는 UI 이벤트와 ROS 구독만 다룬다.
class MainWindowDesign : public QMainWindow, public rclcpp::Node
{
  Q_OBJECT  // Qt의 signal/slot 기능을 쓰려면 클래스 안에 반드시 있어야 함

public:
  explicit MainWindowDesign(QWidget * parent = nullptr);
  ~MainWindowDesign() override;

private slots:
  // QTimer가 주기적으로 호출: ROS2 콜백들을 처리
  void spinRos();

  // 라디오 버튼으로 조절 대상이 바뀌면 슬라이더를 해당 대상의 범위로 갱신
  void onObjectSelected();

  // 슬라이더가 움직이면 선택된 대상의 HSV 범위를 갱신
  void onSliderChanged();

private:
  // "/image_raw" 토픽에서 새 프레임이 도착할 때 호출되는 ROS2 콜백
  void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg);

  // 최신 프레임을 검출기에 넘기고 결과를 화면에 표시
  void updateDisplay();

  // 슬라이더 위치와 옆의 숫자 라벨을 range 값으로 맞춤
  void loadSlidersFromRange(const vision::HsvRange & range);

  // 슬라이더 현재 값으로 만든 HSV 범위
  vision::HsvRange rangeFromSliders() const;

  // 현재 선택된 라디오 버튼에 해당하는 대상
  vision::ObjectId selectedObject() const;

  Ui::MainWindowDesign * ui_;   // .ui에서 만든 위젯들(ui_->USB_screen 등)에 접근하는 포인터

  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;  // 카메라 구독자
  QTimer * ros_timer_;          // spin_some()을 주기 실행시키는 타이머

  cv::Mat latest_frame_;        // 가장 최근에 받은 원본 카메라 프레임(BGR)
  vision::ConeDetector detector_;  // 영상처리 담당
};

#endif  // MAIN_WINDOW_HPP_
