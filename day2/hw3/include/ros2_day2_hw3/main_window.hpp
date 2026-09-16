#ifndef ros2_day2_hw3_MAIN_WINDOW_H
#define ros2_day2_hw3_MAIN_WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QString>
#include <QTimer>
#include "qnode.hpp"
#include "ui_mainwindow.h"

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

  QLabel* value_labels_[TOPIC_COUNT];
  QLabel* state_labels_[TOPIC_COUNT];

private slots:
  void onTopicValueUpdated(int idx, QString value);
  void onTopicStateChanged(int idx, bool alive);
  void onRestartClicked();
  void onRestartResult(bool success, QString message);
};

#endif  // ros2_day2_hw3_MAIN_WINDOW_H
