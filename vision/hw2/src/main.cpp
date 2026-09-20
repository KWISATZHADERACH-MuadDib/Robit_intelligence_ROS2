#include <QApplication>
#include <rclcpp/rclcpp.hpp>
#include "main_window.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);       // ROS2 초기화

  QApplication app(argc, argv);   // Qt 애플리케이션 초기화

  MainWindowDesign window;        // QMainWindow + rclcpp::Node 동시 생성
  window.show();

  int result = app.exec();        // Qt 이벤트 루프 시작 (여기서 QTimer가 spinRos()를 계속 호출)

  rclcpp::shutdown();             // ROS2 종료
  return result;
}