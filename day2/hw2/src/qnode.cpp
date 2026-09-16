#include "../include/ros2_day2_hw2/qnode.hpp"

// ROS2 초기화, publisher/subscriber/client 생성, 통신 스레드 시작
QNode::QNode()
{
  int argc = 0;
  char** argv = NULL;
  rclcpp::init(argc, argv);

  node = rclcpp::Node::make_shared("ros2_day2_hw2");

  cmd_vel_pub_ = node->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

  teleport_client_ = node->create_client<turtlesim::srv::TeleportAbsolute>("/turtle1/teleport_absolute");

  pose_sub_ = node->create_subscription<turtlesim::msg::Pose>(
      "/turtle1/pose", 10,
      [this](const turtlesim::msg::Pose::SharedPtr msg) {
        Q_EMIT poseUpdated(msg->x, msg->y, msg->theta);
      });

  this->start();
}

// ROS2 종료 처리
QNode::~QNode()
{
  if (rclcpp::ok())
  {
    rclcpp::shutdown();
  }
}

// 별도 스레드에서 ROS2 이벤트를 계속 처리 (100Hz)
void QNode::run()
{
  rclcpp::WallRate loop_rate(100);

  while (rclcpp::ok())
  {
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  rclcpp::shutdown();
  Q_EMIT rosShutDown();
}

// cmd_vel 발행 (WASD 이동)
void QNode::publishVelocity(double linear, double angular)
{
  auto msg = geometry_msgs::msg::Twist();
  msg.linear.x = linear;
  msg.angular.z = angular;
  cmd_vel_pub_->publish(msg);
}

// teleport_absolute 서비스 호출 (재생 시 위치 재현)
void QNode::teleportTo(double x, double y, double theta)
{
  if (!teleport_client_->wait_for_service(std::chrono::milliseconds(200)))
  {
    return;
  }

  auto request = std::make_shared<turtlesim::srv::TeleportAbsolute::Request>();
  request->x = x;
  request->y = y;
  request->theta = theta;

  teleport_client_->async_send_request(request);
}