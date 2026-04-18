#include "ImageProcessor.hpp"

ImageProcessor::ImageProcessor() : currentMode(NORMAL) {}

void ImageProcessor::setMode(Mode newMode) {
    currentMode = newMode;
}

cv::Mat ImageProcessor::process(const cv::Mat& input) {
    if (input.empty()) return input;

    cv::Mat output;
    switch (currentMode) {
        case GRAYSCALE:
            cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
            cv::cvtColor(output, output, cv::COLOR_GRAY2BGR); 
            break;
        case EDGES:
            cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
            cv::Canny(output, output, 100, 200);
            cv::cvtColor(output, output, cv::COLOR_GRAY2BGR);
            break;
        case BLUR:
            cv::GaussianBlur(input, output, cv::Size(15, 15), 0);
            break;
        case NORMAL:
        default:
            output = input.clone();
            break;
    }
    return output;
}