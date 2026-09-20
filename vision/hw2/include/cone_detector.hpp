#ifndef CONE_DETECTOR_HPP_
#define CONE_DETECTOR_HPP_

#include <opencv2/core.hpp>

#include <array>

#include "detection_types.hpp"

namespace vision
{

// 프레임 한 장에서 흰선/파란선/콘을 찾아 콘 위치까지 판별하는 파이프라인.
// 대상별 HSV 범위를 보관하며, Qt/ROS에는 의존하지 않는다.
class ConeDetector
{
public:
  ConeDetector();

  void setRange(ObjectId id, const HsvRange & range) {ranges_[id] = range;}
  const HsvRange & range(ObjectId id) const {return ranges_[id];}

  // frame: BGR 영상
  DetectionResult detect(const cv::Mat & frame) const;

private:
  std::array<HsvRange, OBJECT_COUNT> ranges_;
};

}  // namespace vision

#endif  // CONE_DETECTOR_HPP_
