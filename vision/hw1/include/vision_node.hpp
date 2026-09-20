#ifndef VISION_DAY1_HW1__VISION_NODE_HPP_
#define VISION_DAY1_HW1__VISION_NODE_HPP_

#include <rclcpp/rclcpp.hpp>          // ROS2 노드 기본 기능
#include <sensor_msgs/msg/image.hpp>  // 카메라 이미지 메시지 타입
#include <cv_bridge/cv_bridge.hpp>      // ROS 이미지 <-> OpenCV Mat 변환
#include <opencv2/opencv.hpp>         // OpenCV 함수들

// 카메라 토픽을 구독해서 OpenCV로 영상 처리하는 노드
class VisionNode : public rclcpp::Node
{
public:
  // 생성자: 구독자(subscriber) 초기화
  VisionNode();

private:
  // 카메라 이미지가 들어올 때마다 호출되는 콜백 함수
  void image_callback(const sensor_msgs::msg::Image::SharedPtr msg);

  // /camera/image_raw 토픽 구독자
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr sub_;
};

#endif  // VISION_DAY1_HW1__VISION_NODE_HPP_