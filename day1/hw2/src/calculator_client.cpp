#include "rclcpp/rclcpp.hpp"
#include "my_cpp_pkg/srv/calculator.hpp"
using namespace std::chrono_literals;

using Calculator = my_cpp_pkg::srv::Calculator;

// main: 커맨드라인 인자(a, b, op)를 받아 calculator 서비스에 요청을 보내고 결과를 출력한다.
int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);

  if (argc != 4) {
    RCLCPP_INFO(rclcpp::get_logger("calculator_client"),
      "사용법: calculator_client <a> <b> <+|-|*|/>");
    rclcpp::shutdown();
    return 1;
  }

  auto node = rclcpp::Node::make_shared("calculator_client");
  auto client = node->create_client<Calculator>("calculator");

  while (!client->wait_for_service(1s)) {
    RCLCPP_INFO(node->get_logger(), "서비스 대기 중...");
  }

  auto request = std::make_shared<Calculator::Request>();
  request->a = std::stoll(argv[1]);
  request->b = std::stoll(argv[2]);
  request->op = argv[3];

  auto future = client->async_send_request(request);
  if (rclcpp::spin_until_future_complete(node, future) ==
      rclcpp::FutureReturnCode::SUCCESS)
  {
    auto res = future.get();
    RCLCPP_INFO(node->get_logger(), "%ld %s %ld = %f (success=%d)",
      request->a, request->op.c_str(), request->b, res->result, res->success);
  } else {
    RCLCPP_ERROR(node->get_logger(), "서비스 호출 실패");
  }

  rclcpp::shutdown();
  return 0;
}
