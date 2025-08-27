#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include "global_count.h"

class CameraException : public std::runtime_error {
public:
    explicit CameraException(const std::string& msg) 
        : std::runtime_error(msg) {}
};

class CameraCapture {
public:
    explicit CameraCapture(GlobalCount& gc, int deviceId = 0);
    ~CameraCapture();

    // Disable copying
    CameraCapture(const CameraCapture&) = delete;
    CameraCapture& operator=(const CameraCapture&) = delete;

    /**
     * @brief Start continuous frame capture in background thread
     */
    void startCapture();

    /**
     * @brief Stop background capture thread
     */
    void stopCapture();

    /**
     * @brief Get the latest captured frame
     * @return Latest captured frame
     */
    cv::Mat getLatestFrame();

    /**
     * @brief Save the latest frame to a file
     * @param filename Output filename
     * @throws CameraException if save fails
     */
    void saveLatestFrame(const std::string& filename);

private:
    void captureThread();
    void takeSnapshot();
    
    std::unique_ptr<cv::VideoCapture> capture_;
    std::atomic<bool> isRunning_{false};
    GlobalCount& gc_;
    std::atomic<bool> isCoolingDown_{false};
    std::thread captureThread_;
    cv::Mat latestFrame_;
    std::mutex frameMutex_;
};
