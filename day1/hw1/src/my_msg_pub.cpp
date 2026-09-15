#include "rclcpp/rclcpp.hpp"
#include "my_cpp_pkg/msg/my_msg.hpp"
using namespace std::chrono_literals;

// MyMsgPub 노드: id, label 필드를 가진 커스텀 메시지 MyMsg를 0.5초마다 발행한다.
class MyMsgPub : public rclcpp::Node {
public:
  MyMsgPub() : Node("my_msg_pub"), count_(0) {
    pub_ = create_publisher<my_cpp_pkg::msg::MyMsg>("my_topic", 10);
    timer_ = create_wall_timer(500ms, [this]{ callback(); });
  }
private:
  // callback: id를 1씩 증가시키며 메시지를 만들어 발행한다.
  void callback() {
    auto msg = my_cpp_pkg::msg::MyMsg();
    msg.id = count_++;
    msg.label = "hello world!";
    pub_->publish(msg);
    RCLCPP_INFO(this->get_logger(), "발행: id=%d, label=\"%s\"", msg.id, msg.label.c_str());
  }
  rclcpp::Publisher<my_cpp_pkg::msg::MyMsg>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
  int count_;
};

int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MyMsgPub>());
  rclcpp::shutdown();
  return 0;
}