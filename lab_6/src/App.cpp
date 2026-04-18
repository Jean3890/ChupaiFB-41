#include "App.hpp"
#include <iostream>

App::App(const std::string& winName) : windowName(winName), isRunning(false) {}

void App::mouseCallback(int event, int x, int y, int flags, void* userdata) {
    App* app = reinterpret_cast<App*>(userdata);
    app->handleMouse(event, x, y);
}

void App::handleMouse(int event, int x, int y) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        clickPoints.push_back(cv::Point(x, y));
        std::cout << "Клік миші на: (" << x << ", " << y << ")" << std::endl;
    }
}

void App::handleKey(char key) {
    switch (key) {
        case 27: // ESC
        case 'q':
        case 'Q':
            isRunning = false;
            break;
        case '0':
            processor.setMode(ImageProcessor::NORMAL);
            std::cout << "Режим: Normal" << std::endl;
            break;
        case '1':
            processor.setMode(ImageProcessor::GRAYSCALE);
            std::cout << "Режим: Grayscale" << std::endl;
            break;
        case '2':
            processor.setMode(ImageProcessor::EDGES);
            std::cout << "Режим: Canny Edges" << std::endl;
            break;
        case '3':
            processor.setMode(ImageProcessor::BLUR);
            std::cout << "Режим: Blur" << std::endl;
            break;
        case 'c': // Очистити малюнки
        case 'C':
            clickPoints.clear();
            break;
    }
}

void App::run() {
    if (!camera.open(0)) {
        return;
    }

    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(windowName, mouseCallback, this);

    isRunning = true;
    std::cout << "Програма запущена. Натисніть 'q' або ESC для виходу." << std::endl;
    std::cout << "Клавіші 0-3 для зміни режимів. Клік мишею для малювання." << std::endl;

    while (isRunning) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) {
            std::cerr << "Помилка отримання кадру!" << std::endl;
            break;
        }

        cv::Mat processedFrame = processor.process(frame);

        for (const auto& pt : clickPoints) {
            cv::circle(processedFrame, pt, 20, cv::Scalar(0, 0, 255), -1); 
        }

        cv::imshow(windowName, processedFrame);

        char key = (char)cv::waitKey(30); 
        if (key >= 0) {
            handleKey(key);
        }
    }

    cv::destroyAllWindows();
}