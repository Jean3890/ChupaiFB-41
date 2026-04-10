#pragma once
#include <opencv2/opencv.hpp>

class CameraProvider {
private:
    cv::VideoCapture cap;

public:
    CameraProvider();
    ~CameraProvider();
    bool open(int deviceID = 0);
    cv::Mat getFrame();
    bool isOpened() const;
};