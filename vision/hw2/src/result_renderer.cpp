#include "result_renderer.hpp"

#include <opencv2/imgproc.hpp>

namespace vision
{
namespace
{
const cv::Scalar kColorWhiteLine(0, 255, 255);   // 노랑 (BGR)
const cv::Scalar kColorBlueLine(255, 0, 255);    // 자홍
const cv::Scalar kColorNeon(255, 0, 0);          // 파랑 박스
const cv::Scalar kColorOrange(0, 0, 255);        // 빨강 박스

void drawLine(cv::Mat & img, const LineInfo & line, const cv::Scalar & color)
{
  if (!line.found) {
    return;
  }
  const double len = img.cols + img.rows;
  const cv::Point2f p(line.fit[2], line.fit[3]);
  const cv::Point2f d(line.fit[0], line.fit[1]);
  cv::line(img, p - d * len, p + d * len, color, 2);
}

void drawCone(cv::Mat & img, const ConeInfo & cone, const cv::Scalar & color)
{
  if (!cone.found) {
    return;
  }
  cv::rectangle(img, cone.box, color, 2);
}
}  // namespace

cv::Mat renderResult(const cv::Mat & frame, const DetectionResult & result)
{
  cv::Mat img = frame.clone();
  drawLine(img, result.white_line, kColorWhiteLine);
  drawLine(img, result.blue_line, kColorBlueLine);
  drawCone(img, result.neon_cone, kColorNeon);
  drawCone(img, result.orange_cone, kColorOrange);
  return img;
}

}  // namespace vision
