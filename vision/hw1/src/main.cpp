#include <opencv2/opencv.hpp>
#include <iostream>
#include "color_masker.hpp"
#include "image_processor.hpp"

int main(int argc, char** argv)
{
    std::string image_path = (argc > 1) ? argv[1] : "images/balls.jpg";

    cv::Mat img = cv::imread(image_path);
    if(img.empty()) {
        std::cerr << "이미지를 읽을 수 없습니다: " << image_path << std::endl;
        return -1;
    }

    ColorMasker masker;
    ImageProcessor processor(masker);
    processor.processAndSave(img, ".");

    std::cout << "완료: 결과 이미지 6장이 현재 디렉토리에 저장되었습니다." << std::endl;

    return 0;
}