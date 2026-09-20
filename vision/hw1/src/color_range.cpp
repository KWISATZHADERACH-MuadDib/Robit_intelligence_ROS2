#include "color_range.hpp"

//빨간색
std::vector<ColorRange> ColorPresets::red() {
    return {
        {cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255)},
        {cv::Scalar(170, 100, 100), cv::Scalar(179, 255, 255)}
    };
}

//초록색
std::vector<ColorRange> ColorPresets::green() {
    return {
        {cv::Scalar(40, 100, 100), cv::Scalar(80, 255, 255)}
    };
}

//파란색
std::vector<ColorRange> ColorPresets::blue() {
    return {
        {cv::Scalar(100, 100, 100), cv::Scalar(130, 255, 255)}
    };
}