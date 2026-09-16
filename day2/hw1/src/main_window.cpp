/**
 * @file /src/main_window.cpp
 *
 * @brief Implementation for the qt gui.
 *
 * @date August 2024
 **/

#include "../include/ros2_day2_test/main_window.hpp"
#include <cmath>
#include <QDebug>
#include <QPushButton>
#include <QString>

// 생성자: UI 및 버튼 연결 초기화
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindowDesign)
{
  ui->setupUi(this);

  connect(ui->key_w, &QPushButton::clicked, this, &MainWindow::onKeyW);
  connect(ui->key_a, &QPushButton::clicked, this, &MainWindow::onKeyA);
  connect(ui->key_s, &QPushButton::clicked, this, &MainWindow::onKeyS);
  connect(ui->key_d, &QPushButton::clicked, this, &MainWindow::onKeyD);

  connect(ui->circle, &QPushButton::clicked, this, &MainWindow::onCircle);
  connect(ui->rectangle, &QPushButton::clicked, this, &MainWindow::onRectangle);
  connect(ui->triangle, &QPushButton::clicked, this, &MainWindow::onTriangle);

  connect(ui->red, &QPushButton::clicked, this, &MainWindow::onRed);
  connect(ui->green, &QPushButton::clicked, this, &MainWindow::onGreen);
  connect(ui->blue, &QPushButton::clicked, this, &MainWindow::onBlue);

  connect(ui->thin, &QPushButton::clicked, this, &MainWindow::onThin);
  connect(ui->middle, &QPushButton::clicked, this, &MainWindow::onMiddle);
  connect(ui->thick, &QPushButton::clicked, this, &MainWindow::onThick);

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

  bool ok = QObject::connect(qnode, SIGNAL(poseUpdated(double, double, double)), this,
                              SLOT(onPoseUpdated(double, double, double)));
  qDebug() << "[connect] poseUpdated connection result =" << ok;

  // qnode가 cmd_vel을 발행할 때마다 라벨을 갱신하도록 연결
  QObject::connect(qnode, SIGNAL(cmdVelUpdated(double, double)), this,
                    SLOT(onCmdVelUpdated(double, double)));

  // 시작할 때 라벨 기본값 표시
    ui->cmd_vel->setText("cmd_vel : linear 0.00, angular 0.00");
}

// 창 닫힘 처리
void MainWindow::closeEvent(QCloseEvent* event)
{
  QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent* event)  //키보드 입력을 함수 실행으로 바꿔주는 함수
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

// 소멸자
MainWindow::~MainWindow()
{
  delete ui;
}

// 직진
void MainWindow::onKeyW()
{
  qnode->publishVelocity(2.0, 0.0);
}

// 좌회전
void MainWindow::onKeyA()
{
  qnode->publishVelocity(1.0, 1.0);
}

// 후진
void MainWindow::onKeyS()
{
  qnode->publishVelocity(-2.0, 0.0);
}

// 우회전
void MainWindow::onKeyD()
{
  qnode->publishVelocity(1.0, -1.0);
}

// 각도 정규화(-pi ~ pi)
double MainWindow::normalizeAngle(double angle)
{
  while (angle > M_PI) angle -= 2 * M_PI;
  while (angle < -M_PI) angle += 2 * M_PI;
  return angle;
}

// 이동 시퀀스 시작
void MainWindow::startShapeSequence(const QVector<MoveStep>& steps)
{
  move_queue_.clear();
  for (const auto& s : steps)
  {
    move_queue_.enqueue(s);
  }

  qDebug() << "[shape] sequence started, step count =" << move_queue_.size();

  moving_ = true;
  startNextStep();
}

// 다음 이동 스텝 실행
void MainWindow::startNextStep()
{
  if (move_queue_.isEmpty())
  {
    qDebug() << "[shape] queue empty -> stopping";
    moving_ = false;
    qnode->publishVelocity(0.0, 0.0);
    return;
  }

  current_step_ = move_queue_.dequeue();

  start_x_ = current_x_;
  start_y_ = current_y_;
  last_theta_ = current_theta_;
  accumulated_angle_ = 0.0;

  qDebug() << "[shape] start step type=" << (current_step_.type == MoveType::FORWARD ? "FORWARD" : "TURN")
           << "target=" << current_step_.target
           << "linear=" << current_step_.linear_speed
           << "angular=" << current_step_.angular_speed
           << "start_x_=" << start_x_
           << "start_y_=" << start_y_
           << "remaining in queue=" << move_queue_.size();

  qnode->publishVelocity(current_step_.linear_speed, current_step_.angular_speed);
}

// 위치 갱신 및 이동 진행 처리
void MainWindow::onPoseUpdated(double x, double y, double theta)
{
  current_x_ = x;
  current_y_ = y;
  current_theta_ = theta;
  have_pose_ = true;

  debug_counter_++;
  if (debug_counter_ % 10 == 0)
  {
    qDebug() << "[pose] x=" << x << "y=" << y << "theta=" << theta << "moving_=" << moving_;
  }

  if (!moving_)
  {
    return;
  }

  const double SLOWDOWN_MARGIN = 0.3;
  const double SLOW_SPEED = 0.3;

  double linear_to_send = current_step_.linear_speed;
  double angular_to_send = current_step_.angular_speed;

  if (current_step_.type == MoveType::FORWARD)
  {
    double dx = current_x_ - start_x_;
    double dy = current_y_ - start_y_;
    double dist = std::sqrt(dx * dx + dy * dy);
    double remaining = current_step_.target - dist;

    if (debug_counter_ % 5 == 0)
    {
      qDebug() << "[shape] FORWARD progress dist=" << dist << "target=" << current_step_.target;
    }

    if (dist >= current_step_.target)
    {
      qDebug() << "[shape] FORWARD complete, dist=" << dist;
      startNextStep();
      return;
    }

    if (remaining < SLOWDOWN_MARGIN)
    {
      linear_to_send = SLOW_SPEED;
    }
  }
  else
  {
    double delta = normalizeAngle(current_theta_ - last_theta_);
    accumulated_angle_ += delta;
    last_theta_ = current_theta_;

    double remaining = std::fabs(current_step_.target) - std::fabs(accumulated_angle_);

    if (debug_counter_ % 5 == 0)
    {
      qDebug() << "[shape] TURN progress accumulated=" << accumulated_angle_ << "target=" << current_step_.target;
    }

    if (std::fabs(accumulated_angle_) >= std::fabs(current_step_.target))
    {
      qDebug() << "[shape] TURN complete, accumulated=" << accumulated_angle_;
      startNextStep();
      return;
    }

    if (remaining < SLOWDOWN_MARGIN)
    {
      angular_to_send = (current_step_.angular_speed >= 0 ? 1.0 : -1.0) * SLOW_SPEED;
    }
  }

  qnode->publishVelocity(linear_to_send, angular_to_send);
}

// cmd_vel 라벨 갱신
void MainWindow::onCmdVelUpdated(double linear, double angular)
{
  QString text = QString("cmd_vel : linear %1, angular %2")
                     .arg(linear, 0, 'f', 2)
                     .arg(angular, 0, 'f', 2);
  ui->cmd_vel->setText(text);
}

// 원 그리기
void MainWindow::onCircle()
{
  double linear = 2.0;
  double angular = 1.0;

  QVector<MoveStep> steps;
  steps.append({ MoveType::TURN, 2 * M_PI, linear, angular });

  startShapeSequence(steps);
}

// 사각형 그리기
void MainWindow::onRectangle()
{
  double linear = 2.0;
  double side_length = 2.0;
  double angular = 1.0;
  double turn_angle = M_PI / 2.0;

  QVector<MoveStep> steps;
  for (int i = 0; i < 4; ++i)
  {
    steps.append({ MoveType::FORWARD, side_length, linear, 0.0 });
    steps.append({ MoveType::TURN, turn_angle, 0.0, angular });
  }

  startShapeSequence(steps);
}

// 삼각형 그리기
void MainWindow::onTriangle()
{
  double linear = 2.0;
  double side_length = 2.0;
  double angular = 1.0;
  double turn_angle = 2 * M_PI / 3.0;

  QVector<MoveStep> steps;
  for (int i = 0; i < 3; ++i)
  {
    steps.append({ MoveType::FORWARD, side_length, linear, 0.0 });
    steps.append({ MoveType::TURN, turn_angle, 0.0, angular });
  }

  startShapeSequence(steps);
}

// 펜 색상: 빨강
void MainWindow::onRed()
{
  pen_r_ = 255; pen_g_ = 0; pen_b_ = 0;
  qnode->setPen(pen_r_, pen_g_, pen_b_, pen_width_);
}

// 펜 색상: 초록
void MainWindow::onGreen()
{
  pen_r_ = 0; pen_g_ = 255; pen_b_ = 0;
  qnode->setPen(pen_r_, pen_g_, pen_b_, pen_width_);
}

// 펜 색상: 파랑
void MainWindow::onBlue()
{
  pen_r_ = 0; pen_g_ = 0; pen_b_ = 255;
  qnode->setPen(pen_r_, pen_g_, pen_b_, pen_width_);
}

// 펜 굵기: 얇게
void MainWindow::onThin()
{
  pen_width_ = 1;
  qnode->setPen(pen_r_, pen_g_, pen_b_, pen_width_);
}

// 펜 굵기: 중간
void MainWindow::onMiddle()
{
  pen_width_ = 3;
  qnode->setPen(pen_r_, pen_g_, pen_b_, pen_width_);
}

// 펜 굵기: 두껍게
void MainWindow::onThick()
{
  pen_width_ = 5;
  qnode->setPen(pen_r_, pen_g_, pen_b_, pen_width_);
}
