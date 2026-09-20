#ifndef RESULT_RENDERER_HPP_
#define RESULT_RENDERER_HPP_

#include <opencv2/core.hpp>

#include "detection_types.hpp"

namespace vision
{

// 원본 프레임 위에 검출된 선과 콘 바운딩박스를 그린 새 영상을 반환
cv::Mat renderResult(const cv::Mat & frame, const DetectionResult & result);

}  // namespace vision

#endif  // RESULT_RENDERER_HPP_
