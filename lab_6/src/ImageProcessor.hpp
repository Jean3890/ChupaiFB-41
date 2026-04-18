#pragma once
#include <opencv2/opencv.hpp>

class ImageProcessor {
public:
    enum Mode {
        NORMAL = 0,
        GRAYSCALE,
        EDGES,
        BLUR
    };

    ImageProcessor();
    void setMode(Mode newMode);
    cv::Mat process(const cv::Mat& input);

private:
    Mode currentMode;
};