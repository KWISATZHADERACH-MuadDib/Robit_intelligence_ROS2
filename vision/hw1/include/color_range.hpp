#ifndef COLOR_RANGE_HPP_
#define COLOR_RANGE_HPP_

#include <opencv2/opencv.hpp>
#include <vector>


// 하나의 HSV 색상 범위를 담는 구조체 ColorRange
struct ColorRange {
    cv::Scalar lower;
    cv::Scalar upper;
};

// 색상별로 미리 정의된 HSV 범위 값들ㅇ르 정해준다.
//빨간색은 양끝에 범위가 걸쳐 있어서 2개의 범위로 나타낸다
namespace ColorPresets {
    std::vector<ColorRange> red();
    std::vector<ColorRange> green();
    std::vector<ColorRange> blue();
}

#endif