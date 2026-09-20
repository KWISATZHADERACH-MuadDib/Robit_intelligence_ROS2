#include "vision_node.hpp"

// 생성자 구현: "vision_node"라는 이름으로 노드 생성 + 구독자 등록
VisionNode::VisionNode() : Node("vision_node")
{
  // "/camera/image_raw" 토픽을 큐 크기 10으로 구독
  // 메시지가 들어오면 image_callback 함수가 실행됨
  sub_ = create_subscription<sensor_msgs::msg::Image>(
    "/image_raw", 10,
    std::bind(&VisionNode::image_callback, this, std::placeholders::_1));
}

// 이미지 콜백 함수: ROS 이미지 메시지를 받아서 OpenCV로 처리
void VisionNode::image_callback(const sensor_msgs::msg::Image::SharedPtr msg)
{
  cv_bridge::CvImagePtr cv_ptr;

  // ROS 이미지 메시지 -> OpenCV Mat(BGR8) 변환
  try {
    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
  } catch (cv_bridge::Exception & e) {
    // 변환 실패 시 에러 로그만 남기고 종료
    RCLCPP_ERROR(get_logger(), "cv_bridge error: %s", e.what());
    return;
  }

  cv::Mat img = cv_ptr->image;  // 변환된 원본 이미지(BGR)
  cv::Mat hsv, mask;

  // BGR -> HSV 색공간 변환 (색상 기반 검출을 위해)
  cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

  // 지정한 HSV 범위(여기선 노란색 예시)만 흰색(255), 나머지는 검은색(0)으로 이진화
  cv::inRange(hsv, cv::Scalar(20, 20, 100), cv::Scalar(32, 255, 255), mask);

  // 결과 확인용 창 띄우기
  cv::imshow("mask", mask);
  cv::waitKey(1);  // 1ms 대기 (창 갱신을 위해 필요)
}

// 메인 함수: ROS2 초기화 -> 노드 실행 -> 종료
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);                       // ROS2 시스템 초기화
  rclcpp::spin(std::make_shared<VisionNode>());   // 노드 생성 후 콜백 대기(spin)
  rclcpp::shutdown();                             // 종료 처리
  return 0;
}