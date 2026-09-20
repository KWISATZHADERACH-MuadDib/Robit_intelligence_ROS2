#include "color_masker.hpp"

cv::Mat ColorMasker::createMask(const cv::Mat & hsv_image, const std::vector<ColorRange> & ranges) const
{
    cv::Mat result = cv::Mat::zeros(hsv_image.size(), CV_8UC1); //검은색으로 초기화

    for (const auto & range : ranges) {
        cv::Mat partial_mask;
        cv::inRange(hsv_image, range.lower, range.upper, partial_mask);
        result |= partial_mask; //범위가 여러개인 결과(빨간색)을 OR로 합침
    }

    return result;
}