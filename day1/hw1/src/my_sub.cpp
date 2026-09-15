#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

// MySub 노드: "topic" 토픽을 구독하고, 메시지가 도착할 때마다 내용을 로그로 출력한다.
class MySub : public rclcpp::Node {
public:
  // 생성자: 노드 이름을 "my_sub"으로 등록하고 구독자를 생성한다.
  MySub() : Node("my_sub") {
    sub_ = create_subscription<std_msgs::msg::String>(
      "topic", 10,
      [this](const std_msgs::msg::String & msg) { callback(msg); });
  }

private:
  // callback: 메시지가 수신될 때마다 자동으로 호출되어 내용을 로그로 출력한다.
  void callback(const std_msgs::msg::String & msg) {
    RCLCPP_INFO(this->get_logger(), "받은 메시지: \"%s\"", msg.data.c_str());
  }

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
};

// main: ROS2 통신을 초기화하고 MySub 노드를 실행(spin)한다.
int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MySub>());
  rclcpp::shutdown();
  return 0;
}