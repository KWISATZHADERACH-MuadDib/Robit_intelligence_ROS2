#include "fake_imu.hpp"


// 생성자: 노드가 프로세스로 뜨자마자 실행됨 (Unconfigured 상태로 시작).
// 여기선 파라미터 "존재"만 선언해두고, 실제 값 읽기/퍼블리셔 생성은 on_configure로 미룸.
fake_imu::fake_imu(const rclcpp::NodeOptions & options)
: rclcpp_lifecycle::LifecycleNode("fake_imu", options)
{
  declare_parameter("rate_hz", 10.0);
  declare_parameter("stamp_offset_sec", 10.0);
}

// configure 전이 요청이 들어왔을 때 호출됨: Unconfigured → Inactive
// "설정"만 하는 단계. 퍼블리셔는 여기서 만들지만 아직 타이머는 안 돌림.
CallbackReturn fake_imu::on_configure(const rclcpp_lifecycle::State &) 
{
  rate_hz_ = get_parameter("rate_hz").as_double();
  stamp_offset_sec_ = get_parameter("stamp_offset_sec").as_double();
  if (rate_hz_ <= 0.0) {
    RCLCPP_ERROR(get_logger(), "rate_hz must be > 0 (got %.2f)", rate_hz_);
    return CallbackReturn::FAILURE;  // 실패 시 Unconfigured로 되돌아감 (재시도 가능)
  }
  if (stamp_offset_sec_ < 0.0) {
    RCLCPP_ERROR(get_logger(), "stamp_offset_sec must be >= 0 (got %.2f)", stamp_offset_sec_);
    return CallbackReturn::FAILURE;  // 실패 시 Unconfigured로 되돌아감 (재시도 가능)
  }
  pub_ = create_publisher<sensor_msgs::msg::Imu>("imu", 10);
  
  RCLCPP_INFO(get_logger(), "on_configure: rate = %.1f Hz", rate_hz_);
  return CallbackReturn::SUCCESS;
}

// activate 전이 요청: Inactive → Active
// 실제로 "일하기 시작"하는 단계. 타이머를 여기서 만들어야 Active일 때만 동작함.
CallbackReturn fake_imu::on_activate(const rclcpp_lifecycle::State & state)
{
  // 부모 클래스 구현 호출 필수: 얘가 LifecyclePublisher(pub_)를 내부적으로 활성화시켜줌
  // (이걸 빼먹으면 타이머는 돌아도 publish()가 무시됨)
  LifecycleNode::on_activate(state);

  timer_ = create_timer(
    std::chrono::duration<double>(1.0 / rate_hz_),
    [this]() {publish_message();});
  RCLCPP_INFO(get_logger(), "on_activate");
  return CallbackReturn::SUCCESS;
}

// deactivate 전이 요청: Active → Inactive
// "일 멈추기" 단계. 타이머는 LifecycleNode가 자동으로 관리해주는 대상이 아니라서
// 직접 reset()으로 꺼줘야 함 (안 그러면 콜백이 계속 불림)
CallbackReturn fake_imu::on_deactivate(const rclcpp_lifecycle::State & state) 
{
  LifecycleNode::on_deactivate(state);  // 부모가 pub_를 비활성화(발행 무시 상태로 전환)
  timer_.reset();
  RCLCPP_INFO(get_logger(), "on_deactivate");
  return CallbackReturn::SUCCESS;
}

// cleanup 전이 요청: Inactive → Unconfigured
// configure에서 만든 걸 되돌리는 단계. 퍼블리셔를 없애서 처음 상태로 리셋.
CallbackReturn fake_imu::on_cleanup(const rclcpp_lifecycle::State &) 
{
  pub_.reset();
  RCLCPP_INFO(get_logger(), "on_cleanup");
  return CallbackReturn::SUCCESS;
}

// shutdown 전이 요청: Unconfigured/Inactive/Active 어디서든 → Finalized (종료 상태, 복귀 불가)
// previous_state로 "어디서 왔는지" 알 수 있어서 로그에 남김
CallbackReturn fake_imu::on_shutdown(const rclcpp_lifecycle::State & previous_state)
{
  timer_.reset();
  pub_.reset();
  RCLCPP_INFO(get_logger(), "on_shutdown from [%s]", previous_state.label().c_str());
  return CallbackReturn::SUCCESS;
}

// 타이머가 주기적으로 부르는 실제 작업 함수. Active 상태가 아니면
// pub_->publish()를 호출해도 내부적으로 무시됨 (LifecyclePublisher의 특징)

void fake_imu::publish_message()
{
  sensor_msgs::msg::Imu msg;
  msg.header.stamp = now() - rclcpp::Duration::from_seconds(stamp_offset_sec_);
  pub_->publish(msg);
}