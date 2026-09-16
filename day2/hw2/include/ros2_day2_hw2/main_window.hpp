#ifndef ros2_day2_hw2_MAIN_WINDOW_H
#define ros2_day2_hw2_MAIN_WINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QKeyEvent>
#include <QElapsedTimer>
#include <QTimer>
#include "QIcon"
#include "qnode.hpp"
#include "ui_mainwindow.h"

struct PoseRecord
{
  double x;
  double y;
  double theta;
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

  double current_x_ = 0.0;
  double current_y_ = 0.0;
  double current_theta_ = 0.0;

  QVector<PoseRecord> recorded_path_;
  int replay_index_ = 0;

  static const int SAMPLE_INTERVAL_MS = 200;

  QTimer* record_timer_ = nullptr;
  QTimer* replay_timer_ = nullptr;

  QElapsedTimer record_elapsed_timer_;
  QElapsedTimer replay_elapsed_timer_;

  double total_distance_ = 0.0;

  void updateStatusLabels(double elapsed_sec);

private slots:
  void onKeyW();
  void onKeyA();
  void onKeyS();
  void onKeyD();

  void onPoseUpdated(double x, double y, double theta);

  void onRecordStart();
  void onRecordFinish();
  void onRecordReply();

  void onRecordTick();
  void onReplayTick();
};

#endif  // ros2_day2_hw2_MAIN_WINDOW_H