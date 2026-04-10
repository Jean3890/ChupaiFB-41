#pragma once
#include "CameraProvider.hpp"
#include "ImageProcessor.hpp"
#include <string>
#include <vector>

class App {
private:
    CameraProvider camera;
    ImageProcessor processor;
    std::string windowName;
    bool isRunning;
    std::vector<cv::Point> clickPoints; 

    static void mouseCallback(int event, int x, int y, int flags, void* userdata);
    void handleMouse(int event, int x, int y);
    void handleKey(char key);

public:
    App(const std::string& winName);
    void run();
};