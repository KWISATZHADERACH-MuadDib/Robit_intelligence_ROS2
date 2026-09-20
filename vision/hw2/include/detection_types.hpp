#ifndef DETECTION_TYPES_HPP_
#define DETECTION_TYPES_HPP_

#include <opencv2/core.hpp>

#include <array>

namespace vision
{

// HSV 범위 (OpenCV 기준: H 0~179, S/V 0~255)
struct HsvRange
{
  int h_low, h_high;
  int s_low, s_high;
  int v_low, v_high;
};

// 검출 대상 (마스크/HSV 범위를 대상별로 구분하는 인덱스)
enum ObjectId
{
  WHITE_LINE = 0,
  BLUE_LINE,
  NEON_CONE,
  ORANGE_CONE,
  OBJECT_COUNT
};

// 마스크에서 찾은 직선 (cv::fitLine 결과 + 두께)
struct LineInfo
{
  bool found = false;
  cv::Vec4f fit;          // (vx, vy, x0, y0)
  float thickness = 0.f;  // 선의 대략적인 굵기(px)
};

// 마스크에서 찾은 콘 (가장 큰 덩어리의 바운딩박스)
struct ConeInfo
{
  bool found = false;
  cv::Rect box;
};

enum class Vertical { Up, Center, Down };
enum class Horizontal { Left, Center, Right };

// 콘의 위치 (세로선 기준 좌/우, 가로선 기준 위/아래)
struct ConePosition
{
  bool found = false;
  Vertical vertical = Vertical::Center;
  Horizontal horizontal = Horizontal::Center;
};

inline const char * toString(Vertical v)
{
  return v == Vertical::Up ? "up" : (v == Vertical::Down ? "down" : "center");
}

inline const char * toString(Horizontal h)
{
  return h == Horizontal::Left ? "left" : (h == Horizontal::Right ? "right" : "center");
}

// 한 프레임을 분석한 결과 전체
struct DetectionResult
{
  std::array<cv::Mat, OBJECT_COUNT> masks;  // 대상별 이진 마스크
  LineInfo white_line;
  LineInfo blue_line;
  ConeInfo neon_cone;
  ConeInfo orange_cone;
  ConePosition neon_position;
  ConePosition orange_position;
};

}  // namespace vision

#endif  // DETECTION_TYPES_HPP_
