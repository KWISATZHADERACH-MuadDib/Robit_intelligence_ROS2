#ifndef COLOR_MASKER_HPP_
#define COLOR_MASKER_HPP_

#include <opencv2/opencv.hpp>
#include <vector>
#include "color_range.hpp"

//HSV 이미지와 색상 범위를 받아 이진화 마스크를 만든다.
class ColorMasker 
{
public:
    //ranges에 있는 모든 범위를 inRange로 검출 후 OR로 합쳐서 반환한다.
    cv::Mat createMask(const cv::Mat & hsv_image, const std::vector<ColorRange> & ranges) const;
};

#endif