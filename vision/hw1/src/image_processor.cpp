#include "image_processor.hpp"
#include "color_range.hpp"

ImageProcessor::ImageProcessor(const ColorMasker & masker) : masker_(masker)
{

}

void ImageProcessor::processAndSave(const cv::Mat & original, const std::string & output_dir)const{
    //블러 없는 원본으로 마스킹
    maskAndSave(original, output_dir, "");

    //가우시안 블러 적용 후 마스킹
    cv::Mat blurred;
    cv::GaussianBlur(original, blurred, cv::Size(5,5), 0);
    maskAndSave(blurred, output_dir, "_blur");
}

void ImageProcessor::maskAndSave(const cv::Mat & bgr_image, const std::string & output_dir, const std::string & suffix) const
{
    cv::Mat hsv;
    cv::cvtColor(bgr_image, hsv, cv::COLOR_BGR2HSV);

    cv::Mat red_mask = masker_.createMask(hsv, ColorPresets::red());
    cv::Mat green_mask = masker_.createMask(hsv, ColorPresets::green());
    cv::Mat blue_mask = masker_.createMask(hsv, ColorPresets::blue());
    
    cv::imwrite(output_dir + "/red_mask" + suffix + ".png", red_mask);
    cv::imwrite(output_dir + "/green_mask" + suffix + ".png", green_mask);
    cv::imwrite(output_dir + "/blue_mask" + suffix + ".png", blue_mask);
}