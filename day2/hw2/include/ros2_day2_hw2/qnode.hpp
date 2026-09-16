#ifndef ros2_day2_hw2_QNODE_HPP_
#define ros2_day2_hw2_QNODE_HPP_

#ifndef Q_MOC_RUN
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/msg/pose.hpp>
#include <turtlesim/srv/teleport_absolute.hpp>
#endif
#include <QThread>

class QNode : public QThread
{
  Q_OBJECT
public:
  QNode();
  ~QNode();

  void publishVelocity(double linear, double angular);
  void teleportTo(double x, double y, double theta);

protected:
  void run();

private:
  std::shared_ptr<rclcpp::Node> node;

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_;
  rclcpp::Client<turtlesim::srv::TeleportAbsolute>::SharedPtr teleport_client_;

Q_SIGNALS:
  void rosShutDown();
  void poseUpdated(double x, double y, double theta);
};

#endif  // ros2_day2_hw2_QNODE_HPP_