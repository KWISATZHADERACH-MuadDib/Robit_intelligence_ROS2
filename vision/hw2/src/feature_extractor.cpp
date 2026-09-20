#include "feature_extractor.hpp"

#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <vector>

namespace vision
{
namespace
{
// 이 면적(전체 화면 대비 비율)보다 작은 덩어리는 노이즈로 보고 무시
constexpr double kMinConeAreaRatio = 0.001;
constexpr double kMinLineAreaRatio = 0.0005;
}  // namespace

cv::Mat makeMask(const cv::Mat & hsv, const HsvRange & r)
{
  cv::Mat mask;
  cv::inRange(
    hsv, cv::Scalar(r.h_low, r.s_low, r.v_low), cv::Scalar(r.h_high, r.s_high, r.v_high), mask);

  // 열기(침식 -> 팽창): 작은 점 노이즈 제거
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::Mat());
  return mask;
}

LineInfo findLine(const cv::Mat & mask, bool largest_only)
{
  LineInfo line;
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  const double min_area = kMinLineAreaRatio * mask.total();
  std::vector<cv::Point> points;
  double total_area = 0.0;

  if (largest_only) {
    auto it = std::max_element(
      contours.begin(), contours.end(),
      [](const auto & a, const auto & b) {return cv::contourArea(a) < cv::contourArea(b);});
    if (it != contours.end() && cv::contourArea(*it) >= min_area) {
      points = *it;
      total_area = cv::contourArea(*it);
    }
  } else {
    // 파란선은 흰선에 의해 두 조각으로 나뉘므로 충분히 큰 조각을 모두 사용
    for (const auto & c : contours) {
      const double a = cv::contourArea(c);
      if (a >= min_area) {
        points.insert(points.end(), c.begin(), c.end());
        total_area += a;
      }
    }
  }

  if (points.size() < 2) {
    return line;
  }

  cv::fitLine(points, line.fit, cv::DIST_L2, 0, 0.01, 0.01);

  // 굵기 ≈ 면적 / 길이 (길이는 점들의 바운딩박스 긴 변으로 근사)
  const cv::Rect box = cv::boundingRect(points);
  const double length = std::max(box.width, box.height);
  line.thickness = static_cast<float>(length > 0 ? total_area / length : 0.0);
  line.found = true;
  return line;
}

ConeInfo findCone(const cv::Mat & mask)
{
  ConeInfo cone;
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  double best_area = kMinConeAreaRatio * mask.total();
  for (const auto & c : contours) {
    const double a = cv::contourArea(c);
    if (a >= best_area) {
      best_area = a;
      cone.box = cv::boundingRect(c);
      cone.found = true;
    }
  }
  return cone;
}

cv::Mat cropCone(const cv::Mat & frame, const ConeInfo & cone)
{
  if (!cone.found) {
    return cv::Mat();
  }
  return frame(cone.box).clone();
}

}  // namespace vision
