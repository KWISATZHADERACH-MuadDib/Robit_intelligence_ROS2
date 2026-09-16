#ifndef ros2_day2_hw3_QNODE_HPP_
#define ros2_day2_hw3_QNODE_HPP_

#ifndef Q_MOC_RUN
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_srvs/srv/trigger.hpp>
#endif
#include <QThread>
#include <array>
#include <chrono>

static const int TOPIC_COUNT = 3;

class QNode : public QThread
{
  Q_OBJECT
public:
  QNode();
  ~QNode();

  // 재시작 서비스 비동기 호출
  void requestRestart();

protected:
  void run();

private:
  std::shared_ptr<rclcpp::Node> node;

  std::array<rclcpp::Subscription<std_msgs::msg::String>::SharedPtr, TOPIC_COUNT> topic_subs_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr restart_client_;

  std::array<std::chrono::steady_clock::time_point, TOPIC_COUNT> last_stamp_;
  std::array<bool, TOPIC_COUNT> is_alive_;

  static constexpr double TIMEOUT_SEC = 2.0;

  void onTopicMessage(int idx, const std_msgs::msg::String::SharedPtr msg);
  void checkTimeouts();

Q_SIGNALS:
  void rosShutDown();
  void topicValueUpdated(int idx, QString value);
  void topicStateChanged(int idx, bool alive);
  void restartResult(bool success, QString message);
};

#endif  // ros2_day2_hw3_QNODE_HPP_
