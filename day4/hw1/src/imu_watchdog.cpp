#include "imu_watchdog.hpp"

// 생성자: 파라미터 "존재"만 선언. 실제 값 읽기는 on_configure로 미룸 (fake_imu와 같은 이유)
imu_watchdog::imu_watchdog(const rclcpp::NodeOptions & options)
: rclcpp_lifecycle::LifecycleNode("imu_watchdog", options)
{
  declare_parameter("timeout_sec", 0.5);
  declare_parameter("check_rate_hz", 5.0);
}

// configure 전이: Unconfigured → Inactive
// 파라미터 검증 + 구독자/퍼블리셔 생성. 아직 타이머는 안 돌림 (activate에서 시작)
CallbackReturn imu_watchdog::on_configure(const rclcpp_lifecycle::State &)
{
  timeout_sec_ = get_parameter("timeout_sec").as_double();
  check_rate_hz_ = get_parameter("check_rate_hz").as_double();

  // 과제 명세: timeout_sec, check_rate_hz 검증, 0 이하면 FAILURE
  if (timeout_sec_ <= 0.0) {
    RCLCPP_ERROR(get_logger(), "timeout_sec must be > 0 (got %.2f)", timeout_sec_);
    return CallbackReturn::FAILURE;
  }
  if (check_rate_hz_ <= 0.0) {
    RCLCPP_ERROR(get_logger(), "check_rate_hz must be > 0 (got %.2f)", check_rate_hz_);
    return CallbackReturn::FAILURE;
  }

  // fake_imu가 "imu"로 발행하니까 같은 이름으로 구독
  sub_ = create_subscription<sensor_msgs::msg::Imu>(
    "imu", 10, std::bind(&imu_watchdog::on_imu, this, std::placeholders::_1));
  // 신선한 데이터만 걸러서 재발행할 토픽
  pub_ = create_publisher<sensor_msgs::msg::Imu>("imu_checked", 10);

  state_client_ = create_client<GetState>("/fake_imu/get_state");

  RCLCPP_INFO(get_logger(), "on_configure: timeout=%.2fs check_rate=%.1fHz", timeout_sec_, check_rate_hz_);
  return CallbackReturn::SUCCESS;
}

// activate 전이: Inactive → Active
// 여기서부터 실제로 age 검사를 시작함 (타이머 생성)
CallbackReturn imu_watchdog::on_activate(const rclcpp_lifecycle::State & state)
{
  LifecycleNode::on_activate(state);  // 부모가 pub_(LifecyclePublisher)를 활성화
  timer_ = create_timer(
    std::chrono::duration<double>(1.0 / check_rate_hz_),
    [this]() {check_age();});

  state_timer_ = create_timer(
    std::chrono::duration<double>(1.0),
    [this]() {on_state_timer();});

  RCLCPP_INFO(get_logger(), "on_activate");
  return CallbackReturn::SUCCESS;
}

void imu_watchdog::on_state_timer()
{
  if(!state_client_->service_is_ready()) {
    RCLCPP_WARN(get_logger(), "fake_imu get_state not available");
    return;
  }
  if(state_request_pending_) {
    return;   //이전 응답을 아직 못받았으면 새로 보내지 않는다.
  }
  state_request_pending_ = true;

  auto request = std::make_shared<GetState::Request>();
  state_client_->async_send_request(
    request,
    [this](rclcpp::Client<GetState>::SharedFuture future) {
      state_request_pending_ = false;
      RCLCPP_INFO(get_logger(), "fake_imu state: %s",
      future.get()->current_state.label.c_str());
    }
  );
}


// deactivate 전이: Active → Inactive
// age 검사 멈춤. 구독자는 그대로 둬서 메시지는 계속 받되(last_msg_는 갱신됨), 판단만 안 함
CallbackReturn imu_watchdog::on_deactivate(const rclcpp_lifecycle::State & state)
{
  LifecycleNode::on_deactivate(state);
  timer_.reset();
  state_timer_.reset();

  RCLCPP_INFO(get_logger(), "on_deactivate");
  return CallbackReturn::SUCCESS;
}

// cleanup 전이: Inactive → Unconfigured
// configure에서 만든 구독자/퍼블리셔를 전부 해제해서 처음 상태로 되돌림
CallbackReturn imu_watchdog::on_cleanup(const rclcpp_lifecycle::State &)
{
  sub_.reset();
  pub_.reset();
  state_client_.reset();

  RCLCPP_INFO(get_logger(), "on_cleanup");
  return CallbackReturn::SUCCESS;
}

// shutdown 전이: 어느 상태에서든 → Finalized
// 모든 자원 정리 (타이머, 구독자, 퍼블리셔 다)
CallbackReturn imu_watchdog::on_shutdown(const rclcpp_lifecycle::State & previous_state)
{
  timer_.reset();
  sub_.reset();
  pub_.reset();
  RCLCPP_INFO(get_logger(), "on_shutdown from [%s]", previous_state.label().c_str());
  return CallbackReturn::SUCCESS;
}

// /imu 메시지가 들어올 때마다 호출됨. 판단은 여기서 안 하고 저장만 함
// (판단은 타이머 주기로 check_age()에서 따로 하는 구조)
void imu_watchdog::on_imu(const sensor_msgs::msg::Imu::SharedPtr msg)
{
  last_msg_ = msg;
}

// check_rate_hz 주기로 호출됨: 마지막으로 받은 메시지가 오래됐는지 실제로 판단하는 곳
void imu_watchdog::check_age()
{
  State current_state;

  if(!last_msg_) {
     current_state = State::NO_DATA;
  }
  else {
      rclcpp::Time now_time = this->now();
      rclcpp::Time stamp_time(last_msg_->header.stamp);
      rclcpp::Duration age = now_time - stamp_time;

      if(age.seconds() > timeout_sec_) {
        current_state = State::STALE;
      } 
      else {
        current_state = State::FRESH;
      }
  }

  if(current_state != last_state_) {
    if(current_state == State::NO_DATA) {
      RCLCPP_INFO(get_logger(), "[NO DATA]");  //데이터 없음
    }
    else if(current_state == State::STALE) {
      RCLCPP_INFO(get_logger(), "[STALE]");   //오래됨
    }
    else if(current_state == State::FRESH) {
      RCLCPP_INFO(get_logger(), "[RECOVERED]"); //복구됨
    }
    last_state_ = current_state;
  }

  //로그와 별개로 FRESH일 때는 매 틱마다 재발행한다.
  if(current_state == State::FRESH) {
    pub_->publish(*last_msg_);
  }
}