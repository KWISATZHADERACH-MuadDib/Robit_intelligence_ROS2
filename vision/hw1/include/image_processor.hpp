#ifndef IMAGE_PROCESSOR_HPP_
#define IMAGE_PROCESSOR_HPP_

#include <opencv2/opencv.hpp>
#include <string>
#include "color_masker.hpp"

// 원본 이미지를 받아서 RGB 마스크를 상성하고 저장한다
class ImageProcessor
{
public:
    explicit ImageProcessor(const ColorMasker & masker);

    //이미지를 받아 블러 없는 버젼과 블러가 있는 버전의 RGB 마스크를 저장한다.
    void processAndSave(const cv::Mat & original, const std::string & output_dir) const;

private:
    //하나의 RGB 이미지에서 RGB 마스크 3장을 만들어 저장한다.
    void maskAndSave(const cv::Mat & bgr_image, const std::string & output_dir, const std::string & suffix) const;

    ColorMasker masker_;
};

#endif