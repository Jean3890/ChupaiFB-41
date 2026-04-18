#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>

class FaceDetector {
public:
    FaceDetector(const std::string& modelTxt, const std::string& modelBin);
    ~FaceDetector();

    void start();
    void stop();
    
    // UI Thread: Передає кадр
    void setFrame(const cv::Mat& frame);
    
    // UI Thread: Забирає координати
    cv::Rect getFaceBox();

private:
    void workerLoop(); // Worker Thread

    cv::dnn::Net net;
    
    std::thread workerThread;
    std::mutex mtx;
    std::atomic<bool> isRunning;
    std::atomic<bool> hasNewFrame;

    cv::Mat frameToProcess;
    cv::Rect latestFaceBox;
};