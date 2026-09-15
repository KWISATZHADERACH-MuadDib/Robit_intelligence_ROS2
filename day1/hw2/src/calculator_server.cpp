#include "rclcpp/rclcpp.hpp"
#include "my_cpp_pkg/srv/calculator.hpp"

using Calculator = my_cpp_pkg::srv::Calculator;

// CalculatorServer 노드: a, b, op(+, -, *, /)를 받아 사칙연산 결과를 돌려주는 서비스를 제공한다.
class CalculatorServer : public rclcpp::Node {
public:
  CalculatorServer() : Node("calculator_server") {
    srv_ = create_service<Calculator>(
      "calculator",
      [this](const std::shared_ptr<Calculator::Request> req,
             std::shared_ptr<Calculator::Response> res) { calculate(req, res); });
    RCLCPP_INFO(this->get_logger(), "계산기 서비스 서버 준비 완료");
  }

private:
  // calculate: 요청받은 연산자(op)에 따라 a, b를 계산해 결과를 응답에 담는다.
  void calculate(
    const std::shared_ptr<Calculator::Request> req,
    std::shared_ptr<Calculator::Response> res)
  {
    if (req->op == "+") {
      res->result = static_cast<double>(req->a + req->b);
      res->success = true;
    } else if (req->op == "-") {
      res->result = static_cast<double>(req->a - req->b);
      res->success = true;
    } else if (req->op == "*") {
      res->result = static_cast<double>(req->a * req->b);
      res->success = true;
    } else if (req->op == "/") {
      if (req->b == 0) {
        RCLCPP_WARN(this->get_logger(), "0으로 나눌 수 없습니다");
        res->result = 0.0;
        res->success = false;
      } else {
        res->result = static_cast<double>(req->a) / static_cast<double>(req->b);
        res->success = true;
      }
    } else {
      RCLCPP_WARN(this->get_logger(), "알 수 없는 연산자: \"%s\"", req->op.c_str());
      res->result = 0.0;
      res->success = false;
    }

    RCLCPP_INFO(this->get_logger(), "요청: %ld %s %ld -> 결과: %f",
      req->a, req->op.c_str(), req->b, res->result);
  }

  rclcpp::Service<Calculator>::SharedPtr srv_;
};

// main: ROS2 통신을 초기화하고 CalculatorServer 노드를 실행(spin)한다.
int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CalculatorServer>());
  rclcpp::shutdown();
  return 0;
}
