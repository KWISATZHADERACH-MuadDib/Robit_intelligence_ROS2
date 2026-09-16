#include "../include/ros2_day2_hw3/qnode.hpp"

// ROS2 초기화, 토픽 3개 구독 및 재시작 서비스 클라이언트 생성, 통신 스레드 시작
QNode::QNode()
{
  int argc = 0;
  char** argv = nullptr;
  rclcpp::init(argc, argv);

  node = rclcpp::Node::make_shared("ros2_day2_hw3_monitor");

  const std::array<std::string, TOPIC_COUNT> topic_names = { "/topic1", "/topic2", "/topic3" };

  auto now = std::chrono::steady_clock::now();
  for (int i = 0; i < TOPIC_COUNT; ++i)
  {
    last_stamp_[i] = now;
    is_alive_[i] = false;

    topic_subs_[i] = node->create_subscription<std_msgs::msg::String>(
        topic_names[i], 10,
        [this, i](const std_msgs::msg::String::SharedPtr msg) { onTopicMessage(i, msg); });
  }

  restart_client_ = node->create_client<std_srvs::srv::Trigger>("/restart_request");

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

// 별도 스레드에서 ROS2 이벤트를 계속 처리하면서 타임아웃 여부를 주기적으로 점검
void QNode::run()
{
  rclcpp::WallRate loop_rate(20);

  while (rclcpp::ok())
  {
    rclcpp::spin_some(node);
    checkTimeouts();
    loop_rate.sleep();
  }

  rclcpp::shutdown();
  Q_EMIT rosShutDown();
}

// 토픽 메시지 수신 처리: 값 갱신 및 생존 상태 전환 알림
void QNode::onTopicMessage(int idx, const std_msgs::msg::String::SharedPtr msg)
{
  last_stamp_[idx] = std::chrono::steady_clock::now();

  if (!is_alive_[idx])
  {
    is_alive_[idx] = true;
    Q_EMIT topicStateChanged(idx, true);
  }

  Q_EMIT topicValueUpdated(idx, QString::fromStdString(msg->data));
}

// TIMEOUT_SEC 이상 메시지가 없는 토픽을 "응답 없음" 상태로 전환
void QNode::checkTimeouts()
{
  auto now = std::chrono::steady_clock::now();

  for (int i = 0; i < TOPIC_COUNT; ++i)
  {
    double elapsed = std::chrono::duration<double>(now - last_stamp_[i]).count();

    if (is_alive_[i] && elapsed > TIMEOUT_SEC)
    {
      is_alive_[i] = false;
      Q_EMIT topicStateChanged(i, false);
    }
  }
}

// 재시작 서비스 호출: 서비스가 준비되지 않았으면 실패로 즉시 알림
void QNode::requestRestart()
{
  if (!restart_client_->wait_for_service(std::chrono::milliseconds(200)))
  {
    Q_EMIT restartResult(false, "서비스 서버에 연결할 수 없습니다");
    return;
  }

  auto request = std::make_shared<std_srvs::srv::Trigger::Request>();

  restart_client_->async_send_request(
      request, [this](rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future) {
        auto response = future.get();
        Q_EMIT restartResult(response->success, QString::fromStdString(response->message));
      });
}
