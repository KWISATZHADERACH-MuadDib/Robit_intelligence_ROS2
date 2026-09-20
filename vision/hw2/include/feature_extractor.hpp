#ifndef FEATURE_EXTRACTOR_HPP_
#define FEATURE_EXTRACTOR_HPP_

#include <opencv2/core.hpp>

#include "detection_types.hpp"

namespace vision
{

// HSV 영상에서 inRange + 열기 연산으로 노이즈가 제거된 이진 마스크 생성
cv::Mat makeMask(const cv::Mat & hsv, const HsvRange & range);

// 마스크에서 직선 검출 (largest_only: 가장 큰 덩어리만 사용)
LineInfo findLine(const cv::Mat & mask, bool largest_only);

// 마스크에서 가장 큰 덩어리의 바운딩박스 검출
ConeInfo findCone(const cv::Mat & mask);

// 콘의 바운딩박스 내부만 잘라낸 새 이미지 (콘이 없으면 빈 Mat)
cv::Mat cropCone(const cv::Mat & frame, const ConeInfo & cone);

}  // namespace vision

#endif  // FEATURE_EXTRACTOR_HPP_
