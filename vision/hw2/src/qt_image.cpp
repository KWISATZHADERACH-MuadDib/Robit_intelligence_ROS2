#include "qt_image.hpp"

#include <QImage>
#include <QLabel>
#include <QPixmap>

void showMatOnLabel(const cv::Mat & mat, QLabel * label)
{
  if (mat.empty()) {
    return;
  }

  // OpenCV는 BGR, Qt는 RGB 순서라 QImage::Format_BGR888로 지정해서 변환 (색 반전 방지)
  const QImage::Format format =
    mat.channels() == 1 ? QImage::Format_Grayscale8 : QImage::Format_BGR888;
  QImage img(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), format);

  // 라벨 크기에 맞춰 비율 유지하며 축소
  label->setPixmap(
    QPixmap::fromImage(img).scaled(
      label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
