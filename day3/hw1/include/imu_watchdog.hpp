#pragma once

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp_lifecycle/lifecycle_publisher.hpp"
#include "sensor_msgs/msg/imu.hpp"

using CallbackReturn =
  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class imu_watchdog : public rclcpp_lifecycle::LifecycleNode
{
public:
  explicit imu_watchdog(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

  // Lifecycle 전이 콜백 5개 (fake_imu와 동일한 역할)
  CallbackReturn on_configure(const rclcpp_lifecycle::State &) override;
  CallbackReturn on_activate(const rclcpp_lifecycle::State & state) override;
  CallbackReturn on_deactivate(const rclcpp_lifecycle::State & state) override;
  CallbackReturn on_cleanup(const rclcpp_lifecycle::State &) override;
  CallbackReturn on_shutdown(const rclcpp_lifecycle::State & previous_state) override;

private:
  // /imu 구독 콜백: 메시지가 들어올 때마다 호출됨 (수신 즉시 저장만, 판단은 안 함)
  void on_imu(const sensor_msgs::msg::Imu::SharedPtr msg);

  // 타이머가 주기적으로(check_rate_hz마다) 호출: 저장된 마지막 메시지의 신선도를 실제로 판단하는 함수
  void check_age();

  double timeout_sec_{0.0};      // on_configure에서 파라미터로 채워짐: 이 시간 넘으면 STALE
  double check_rate_hz_{0.0};    // on_configure에서 채워짐: check_age()를 부르는 주기

  rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr sub_;              // /imu 구독자
  rclcpp_lifecycle::LifecyclePublisher<sensor_msgs::msg::Imu>::SharedPtr pub_;  // /imu_checked 발행자 (신선할 때만 씀)
  rclcpp::TimerBase::SharedPtr timer_;                                      // age 검사용 타이머 (fake_imu의 발행용 타이머와 역할 다름)

  sensor_msgs::msg::Imu::SharedPtr last_msg_;   // on_imu가 채워주는, 가장 최근에 받은 메시지

  // TODO: NO_DATA/FRESH/STALE 중 "이전 상태"를 기억할 변수 필요
  //       (상태 바뀔 때만 로그 찍으려면 이전 값이랑 비교해야 함)
  enum class State { NO_DATA, FRESH, STALE};
  State last_state_{State::NO_DATA};
};