/**
 * @file /include/ros2_day2_test/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date February 2011
 **/
#ifndef ros2_day2_test_QNODE_HPP_
#define ros2_day2_test_QNODE_HPP_

#ifndef Q_MOC_RUN
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/srv/set_pen.hpp>
#include <turtlesim/msg/pose.hpp>
#endif
#include <QThread>

//Qnode : ROS2 통신을 전담하는 클래스
class QNode : public QThread
{
  Q_OBJECT
public:
  QNode();
  ~QNode();

  // cmd vel 전송
  void publishVelocity(double linear, double angular);
  //색상, 굵기 설정
  void setPen(int r, int g, int b, int width, bool off = false);

protected:
  void run();

private:
  std::shared_ptr<rclcpp::Node> node; //실제 ROS2 노드 인스턴스

  // /turtle1/cmd_vel 토픽으로 속도 명령을 보내는 publisher
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
  // /turtle1/set_pen 서비스를 호출하는 클라이언트
  rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr set_pen_client_;
  // /turtle1/pose 를 구독하는 subscription
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_;

Q_SIGNALS: // GUI에서 이벤트가 발생했다고 알리는 신호들
  void rosShutDown(); //ROS2가 종료되었을 때 GUI에게 알림

  //cmd_vel 값이 바뀔 때마다 GUI에 현재 속도값을 알려줌
  void cmdVelUpdated(double linear, double angular);

  //실제 turtle 위치(x, y, theta)가 갱신될 때마다 GUI에 알려줌
  void poseUpdated(double x, double y, double theta);
};

#endif /* ros2_day2_test_QNODE_HPP_ */
