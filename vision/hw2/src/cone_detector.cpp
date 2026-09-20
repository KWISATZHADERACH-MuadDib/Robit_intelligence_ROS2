#include "cone_detector.hpp"

#include <opencv2/imgproc.hpp>

#include "feature_extractor.hpp"
#include "position_classifier.hpp"

namespace vision
{

ConeDetector::ConeDetector()
{
  // 초기 HSV 범위 (USB 웹캠, 밝기 0 / 수동 노출 환경에서 측정한 값. 슬라이더로 튜닝)
  ranges_[WHITE_LINE] = {0, 179, 0, 50, 100, 255};
  ranges_[BLUE_LINE] = {100, 120, 150, 255, 50, 255};
  ranges_[NEON_CONE] = {35, 60, 120, 255, 60, 255};
  ranges_[ORANGE_CONE] = {0, 15, 150, 255, 120, 255};
}

DetectionResult ConeDetector::detect(const cv::Mat & frame) const
{
  DetectionResult result;

  // 가우시안 블러 후 HSV 변환
  cv::Mat blurred, hsv;
  cv::GaussianBlur(frame, blurred, cv::Size(5, 5), 0);
  cv::cvtColor(blurred, hsv, cv::COLOR_BGR2HSV);

  for (int i = 0; i < OBJECT_COUNT; ++i) {
    result.masks[i] = makeMask(hsv, ranges_[i]);
  }

  result.white_line = findLine(result.masks[WHITE_LINE], true);
  result.blue_line = findLine(result.masks[BLUE_LINE], false);
  result.neon_cone = findCone(result.masks[NEON_CONE]);
  result.orange_cone = findCone(result.masks[ORANGE_CONE]);

  const LineRoles roles = assignLineRoles(result.white_line, result.blue_line, frame.size());
  result.neon_position = classifyCone(result.neon_cone, roles);
  result.orange_position = classifyCone(result.orange_cone, roles);
  return result;
}

}  // namespace vision
