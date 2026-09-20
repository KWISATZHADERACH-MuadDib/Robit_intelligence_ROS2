#ifndef QT_IMAGE_HPP_
#define QT_IMAGE_HPP_

#include <opencv2/core.hpp>

class QLabel;

// OpenCV Mat(BGR 3채널 또는 Gray 1채널)을 QLabel 크기에 맞춰 비율 유지로 표시
void showMatOnLabel(const cv::Mat & mat, QLabel * label);

#endif  // QT_IMAGE_HPP_
