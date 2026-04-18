#include <opencv2/opencv.hpp>
#include "FaceDetector.hpp"
#include <iostream>

int main() {
    // Ініціалізація асинхронного детектора
    FaceDetector detector("deploy.prototxt", "res10_300x300_ssd_iter_140000.caffemodel");
    detector.start();

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Помилка: Не вдалося відкрити камеру!" << std::endl;
        return -1;
    }

    cv::Mat frame;
    double t = (double)cv::getTickCount();
    int fpsCounter = 0;
    double currentFPS = 0.0;

    std::cout << "Камеру запущено. Натисніть ESC для виходу." << std::endl;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // 1. Відправляємо копію в фоновий потік (це швидко, відео не гальмує)
        detector.setFrame(frame);

        // 2. Отримуємо останні ВІДОМІ координати
        cv::Rect face = detector.getFaceBox();

        // 3. Малюємо рамку (якщо обличчя знайдено)
        if (face.area() > 0) {
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
            cv::putText(frame, "Face Detected", cv::Point(face.x, face.y - 10), 
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
        }

        // --- Рахуємо і малюємо FPS (щоб довести викладачу, що він високий) ---
        fpsCounter++;
        if (fpsCounter == 10) {
            t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
            currentFPS = 10.0 / t;
            t = (double)cv::getTickCount();
            fpsCounter = 0;
        }
        cv::putText(frame, "UI FPS: " + std::to_string(static_cast<int>(currentFPS)), 
                    cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 0, 0), 2);

        // 4. Показуємо кадр
        cv::imshow("Async Face Detection", frame);

        if (cv::waitKey(1) == 27) { // ESC
            break;
        }
    }

    detector.stop();
    return 0;
}