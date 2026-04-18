#include "CameraProvider.hpp"
#include <iostream>

CameraProvider::CameraProvider() {}

CameraProvider::~CameraProvider() {
    if (cap.isOpened()) {
        cap.release();
    }
}

bool CameraProvider::open(int deviceID) {
    cap.open(deviceID);
    if (!cap.isOpened()) {
        std::cerr << "не вдалося відкрити камеру з ID " << deviceID << std::endl;
        return false;
    }
    return true;
}

cv::Mat CameraProvider::getFrame() {
    cv::Mat frame;
    if (cap.isOpened()) {
        cap.read(frame);
    }
    return frame;
}

bool CameraProvider::isOpened() const {
    return cap.isOpened();
}