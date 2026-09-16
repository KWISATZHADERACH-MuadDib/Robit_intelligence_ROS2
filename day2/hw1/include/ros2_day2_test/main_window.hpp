/**
 * @file /include/ros2_day2_test/main_window.hpp
 *
 * @brief Qt based gui for %(package)s.
 *
 * @date August 2024
 **/

#ifndef ros2_day2_test_MAIN_WINDOW_H
#define ros2_day2_test_MAIN_WINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QQueue>
#include <QKeyEvent>
#include "QIcon"
#include "qnode.hpp"
#include "ui_mainwindow.h"

enum class MoveType
{
  FORWARD,
  TURN
};

struct MoveStep
{
  MoveType type;
  double target;
  double linear_speed;
  double angular_speed;
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();
  QNode* qnode;

private:
  Ui::MainWindowDesign* ui;
  void closeEvent(QCloseEvent* event);
  void keyPressEvent(QKeyEvent* event);

  int pen_r_ = 255;
  int pen_g_ = 0;
  int pen_b_ = 0;
  int pen_width_ = 3;

  double current_x_ = 0.0;
  double current_y_ = 0.0;
  double current_theta_ = 0.0;
  bool have_pose_ = false;

  double start_x_ = 0.0;
  double start_y_ = 0.0;
  double last_theta_ = 0.0;
  double accumulated_angle_ = 0.0;

  bool moving_ = false;
  MoveStep current_step_{};
  QQueue<MoveStep> move_queue_;

  int debug_counter_ = 0;

  void startShapeSequence(const QVector<MoveStep>& steps);
  void startNextStep();
  static double normalizeAngle(double angle);

private slots:
  void onKeyW();
  void onKeyA();
  void onKeyS();
  void onKeyD();

  void onCircle();
  void onRectangle();
  void onTriangle();

  void onRed();
  void onGreen();
  void onBlue();

  void onThick();
  void onMiddle();
  void onThin();

  void onPoseUpdated(double x, double y, double theta);
  void onCmdVelUpdated(double linear, double angular);  // cmd_vel 라벨 갱신용
};

#endif  // ros2_day2_test_MAIN_WINDOW_H
