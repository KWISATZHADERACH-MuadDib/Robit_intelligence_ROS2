#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <chrono>

using namespace std::chrono_literals;

// 대시보드 동작 확인용 테스트 퍼블리셔.
// /topic1 은 1초 간격(항상 "정상")으로,
// /topic2 는 3초 간격, /topic3 은 5초 간격으로 발행해 서로 다른 갱신 속도를 보여준다.
// 2초 타임아웃보다 느린 topic2/topic3 은 "정상"/"응답 없음"이 주기적으로 반복된다.
class TopicTestPublisher : public rclcpp::Node
{
public:
  TopicTestPublisher() : Node("topic_test_publisher"), count1_(0), count3_(0)
  {
    pub1_ = create_publisher<std_msgs::msg::String>("/topic1", 10);
    pub2_ = create_publisher<std_msgs::msg::String>("/topic2", 10);
    pub3_ = create_publisher<std_msgs::msg::String>("/topic3", 10);

    timer1_ = create_wall_timer(1s, [this]() {
      auto msg = std_msgs::msg::String();
      msg.data = "topic1 count=" + std::to_string(count1_++);
      pub1_->publish(msg);
    });

    timer2_ = create_wall_timer(3s, [this]() {
      auto msg = std_msgs::msg::String();
      msg.data = "hello world!";
      pub2_->publish(msg);
    });

    timer3_ = create_wall_timer(5s, [this]() {
      auto msg = std_msgs::msg::String();
      msg.data = "topic3 count=" + std::to_string(count3_++);
      pub3_->publish(msg);
    });
  }

private:
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub1_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub2_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub3_;
  rclcpp::TimerBase::SharedPtr timer1_;
  rclcpp::TimerBase::SharedPtr timer2_;
  rclcpp::TimerBase::SharedPtr timer3_;
  int count1_;
  int count3_;
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TopicTestPublisher>());
  rclcpp::shutdown();
  return 0;
}
