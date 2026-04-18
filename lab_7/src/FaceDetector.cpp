#include "FaceDetector.hpp"
#include <chrono>
#include <iostream>

FaceDetector::FaceDetector(const std::string& modelTxt, const std::string& modelBin) {
    net = cv::dnn::readNetFromCaffe(modelTxt, modelBin);
    isRunning = false;
    hasNewFrame = false;
}

FaceDetector::~FaceDetector() {
    stop();
}

void FaceDetector::start() {
    if (!isRunning) {
        isRunning = true;
        workerThread = std::thread(&FaceDetector::workerLoop, this);
    }
}

void FaceDetector::stop() {
    isRunning = false;
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void FaceDetector::setFrame(const cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(mtx);
    frame.copyTo(frameToProcess); // Копіюємо кадр ПІД м'ютексом
    hasNewFrame = true;
}

cv::Rect FaceDetector::getFaceBox() {
    std::lock_guard<std::mutex> lock(mtx);
    return latestFaceBox; // Віддаємо координати ПІД м'ютексом
}

void FaceDetector::workerLoop() {
    while (isRunning) {
        cv::Mat frame;
        
        // 1. Забираємо кадр для обробки
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (hasNewFrame && !frameToProcess.empty()) {
                frameToProcess.copyTo(frame);
                hasNewFrame = false;
            }
        }

        // 2. Якщо є новий кадр - робимо інференс
        if (!frame.empty()) {
            // ШТУЧНА ЗАТРИМКА (Для демонстрації 2-го рівня)
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0), false, false);
            net.setInput(blob);
            cv::Mat detection = net.forward();

            cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

            cv::Rect faceBox;
            float maxConfidence = 0.0;

            // Шукаємо обличчя з найвищою впевненістю
            for (int i = 0; i < detectionMat.rows; i++) {
                float confidence = detectionMat.at<float>(i, 2);
                if (confidence > 0.5 && confidence > maxConfidence) {
                    maxConfidence = confidence;
                    int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
                    int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
                    int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
                    int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);
                    faceBox = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));
                }
            }

            // 3. Записуємо результат ПІД м'ютексом
            {
                std::lock_guard<std::mutex> lock(mtx);
                latestFaceBox = faceBox;
            }
        } else {
            // Якщо нових кадрів немає, спимо 10мс, щоб не "палити" процесор циклом
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}