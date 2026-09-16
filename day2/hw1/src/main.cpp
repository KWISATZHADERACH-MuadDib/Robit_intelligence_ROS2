#include <QApplication>
#include <iostream>

#include "../include/ros2_day2_test/main_window.hpp"

// 프로그램 시작점
int main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
