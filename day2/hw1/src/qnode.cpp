/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date August 2024
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

#include "../include/ros2_day2_test/qnode.hpp"

// 생성자: ROS2 초기화
QNode::QNode()
{
  int argc = 0;
  char** argv = NULL;
  rclcpp::init(argc, argv); //ROS2 시스템 초기화

  //"ros2_day2_test"라는 이름으로 ROS2 노드 생성
  node = rclcpp::Node::make_shared("ros2_day2_test");
 
  // 서비스 클라이언트 생성
  cmd_vel_pub_ = node->create_publisher<geometry_msgs::msg::Twist> ("/turtle1/cmd_vel", 10);
 
  // t서비스 클라이언트 생성
  set_pen_client_ = node->create_client<turtlesim::srv::SetPen>("/turtle1/set_pen");

  // 실제 turtle 위치(pose)를 구독해서 바뀔 때마다 GUI로 signal 보냄
  pose_sub_ = node->create_subscription<turtlesim::msg::Pose>(
      "/turtle1/pose", 10,
      [this](const turtlesim::msg::Pose::SharedPtr msg) {
        Q_EMIT poseUpdated(msg->x, msg->y, msg->theta);
      });

  //publisher, client를 다 만든 다음에 스레드를 시작해야함.
  // this->start()가 호출되면 run()이 별도 스레드에서 바로 돌기 시작한다.
  this->start();
}

// 소멸자: ROS2 종료
QNode::~QNode()
{
  if (rclcpp::ok())
  {
    rclcpp::shutdown();
  }
}

// ROS2 이벤트 처리 스레드
void QNode::run()
{
  rclcpp::WallRate loop_rate(100); //초당 20번 주기로 반복하겠다는 설정

  while (rclcpp::ok())  //ROS2가 살아있는 동안 계속 반복
  {
    rclcpp::spin_some(node);  //대기중인 ROS2 이벤트 처리
    loop_rate.sleep();
  }

  rclcpp::shutdown();
  Q_EMIT rosShutDown(); //GUI 스레드에 "ROS2 종료됨" 신호를 보냄
}

// 속도 명령 발행
void QNode::publishVelocity(double linear, double angular)
{
  auto msg = geometry_msgs::msg::Twist(); //빈 twist 메세지 생성
  msg.linear.x = linear;  //전진, 후진 속도
  msg.angular.z = angular;  //회전 속도
  cmd_vel_pub_->publish(msg); //토픽에 발행

  //GUI에서 명령을 보냈다고 알림
  Q_EMIT cmdVelUpdated(linear, angular);
}

// 펜 색상/굵기 설정 서비스 호출
void QNode::setPen(int r, int g, int b, int width, bool off)
{
  //서비스 서버가 200ms 안에 응답 가능한 상태인지 확인
  if (!set_pen_client_->wait_for_service(std::chrono::milliseconds(200))) {
    return ;  //서비스가 아직 준비 안 됐으면 그냥 포기(에러 방지)
  }

  //서비스 요청 메세지 생성 및 값 채우기
  auto request = std::make_shared<turtlesim::srv::SetPen::Request>();
  request->r = r; //빨강
  request->g = g; //초록
  request->b = b; //파랑
  request->width = width; //굵기
  request->off = off; //true면 펜을 때서 선을 그리지 않음
  
  //비동기로 서비스 요청 전송(응답을 기다리지 않고 바로 다음 코드로 진행)
  set_pen_client_->async_send_request(request);
}