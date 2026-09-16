#include "../include/ros2_day2_hw2/main_window.hpp"
#include <cmath>
#include <QPushButton>
#include <QString>

// UI 세팅, 버튼 연결, 키보드 포커스 설정, QNode 생성, 타이머 준비
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindowDesign)
{
  ui->setupUi(this);

  connect(ui->key_w, &QPushButton::clicked, this, &MainWindow::onKeyW);
  connect(ui->key_a, &QPushButton::clicked, this, &MainWindow::onKeyA);
  connect(ui->key_s, &QPushButton::clicked, this, &MainWindow::onKeyS);
  connect(ui->key_d, &QPushButton::clicked, this, &MainWindow::onKeyD);

  connect(ui->record_start, &QPushButton::clicked, this, &MainWindow::onRecordStart);
  connect(ui->record_finish, &QPushButton::clicked, this, &MainWindow::onRecordFinish);
  connect(ui->record_reply, &QPushButton::clicked, this, &MainWindow::onRecordReply);

  QList<QPushButton*> all_buttons = this->findChildren<QPushButton*>();
  for (QPushButton* btn : all_buttons)
  {
    btn->setFocusPolicy(Qt::NoFocus);
  }
  this->setFocusPolicy(Qt::StrongFocus);
  this->setFocus();

  QIcon icon("://ros-icon.png");
  this->setWindowIcon(icon);

  qnode = new QNode();

  QObject::connect(qnode, SIGNAL(rosShutDown()), this, SLOT(close()));
  QObject::connect(qnode, SIGNAL(poseUpdated(double, double, double)), this,
                    SLOT(onPoseUpdated(double, double, double)));

  record_timer_ = new QTimer(this);
  connect(record_timer_, &QTimer::timeout, this, &MainWindow::onRecordTick);

  replay_timer_ = new QTimer(this);
  connect(replay_timer_, &QTimer::timeout, this, &MainWindow::onReplayTick);

  ui->state_print->setText("대기 중");
  ui->distance_print->setText("0.00 m");
  ui->time_print->setText("0.0 s");
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  QMainWindow::closeEvent(event);
}

// WASD 키 입력 처리
void MainWindow::keyPressEvent(QKeyEvent* event)
{
  switch (event->key())
  {
    case Qt::Key_W:
      onKeyW();
      break;
    case Qt::Key_A:
      onKeyA();
      break;
    case Qt::Key_S:
      onKeyS();
      break;
    case Qt::Key_D:
      onKeyD();
      break;
    default:
      QMainWindow::keyPressEvent(event);
      break;
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}

// 전진
void MainWindow::onKeyW()
{
  qnode->publishVelocity(2.0, 0.0);
}

// 좌회전 이동
void MainWindow::onKeyA()
{
  qnode->publishVelocity(1.0, 1.0);
}

// 후진
void MainWindow::onKeyS()
{
  qnode->publishVelocity(-2.0, 0.0);
}

// 우회전 이동
void MainWindow::onKeyD()
{
  qnode->publishVelocity(1.0, -1.0);
}

// 현재 위치 갱신 (기록은 record_timer_가 별도로 담당)
void MainWindow::onPoseUpdated(double x, double y, double theta)
{
  current_x_ = x;
  current_y_ = y;
  current_theta_ = theta;
}

// 거리/시간 라벨 갱신
void MainWindow::updateStatusLabels(double elapsed_sec)
{
  ui->distance_print->setText(QString::number(total_distance_, 'f', 2) + " m");
  ui->time_print->setText(QString::number(elapsed_sec, 'f', 1) + " s");
}

// 기록 초기화 후 기록 타이머 시작
void MainWindow::onRecordStart()
{
  replay_timer_->stop();

  recorded_path_.clear();
  replay_index_ = 0;
  total_distance_ = 0.0;

  record_elapsed_timer_.start();
  record_timer_->start(SAMPLE_INTERVAL_MS);

  ui->state_print->setText("기록 중");
  updateStatusLabels(0.0);
}

// 기록 타이머 정지
void MainWindow::onRecordFinish()
{
  record_timer_->stop();

  ui->state_print->setText(QString("기록 종료 (%1개 지점)").arg(recorded_path_.size()));
}

// 현재 위치를 경로에 추가하고 거리/시간 갱신
void MainWindow::onRecordTick()
{
  PoseRecord rec{ current_x_, current_y_, current_theta_ };

  if (!recorded_path_.isEmpty())
  {
    double dx = current_x_ - recorded_path_.last().x;
    double dy = current_y_ - recorded_path_.last().y;
    total_distance_ += std::sqrt(dx * dx + dy * dy);
  }

  recorded_path_.append(rec);

  double elapsed_sec = record_elapsed_timer_.elapsed() / 1000.0;
  updateStatusLabels(elapsed_sec);
}

// 재생 초기화 후 재생 타이머 시작
void MainWindow::onRecordReply()
{
  record_timer_->stop();

  if (recorded_path_.isEmpty())
  {
    ui->state_print->setText("기록된 경로가 없습니다");
    return;
  }

  replay_index_ = 0;
  total_distance_ = 0.0;
  replay_elapsed_timer_.start();

  ui->state_print->setText("재생 중");
  updateStatusLabels(0.0);

  replay_timer_->start(SAMPLE_INTERVAL_MS);
}

// 다음 기록 지점으로 순간이동시키며 재생 진행
void MainWindow::onReplayTick()
{
  if (replay_index_ >= recorded_path_.size())
  {
    replay_timer_->stop();
    ui->state_print->setText("재생 완료");
    return;
  }

  const PoseRecord& rec = recorded_path_[replay_index_];
  qnode->teleportTo(rec.x, rec.y, rec.theta);

  if (replay_index_ > 0)
  {
    double dx = rec.x - recorded_path_[replay_index_ - 1].x;
    double dy = rec.y - recorded_path_[replay_index_ - 1].y;
    total_distance_ += std::sqrt(dx * dx + dy * dy);
  }

  double elapsed_sec = replay_elapsed_timer_.elapsed() / 1000.0;
  updateStatusLabels(elapsed_sec);

  replay_index_++;
}