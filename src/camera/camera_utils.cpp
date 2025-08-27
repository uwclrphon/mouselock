#include "camera_utils.h"
#include <opencv2/imgcodecs.hpp>

CameraCapture::CameraCapture(GlobalCount& gc, int deviceId) : gc_(gc) {
    // Try to open camera with retry
    int maxAttempts = 3;
    for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
        try {
            capture_ = std::make_unique<cv::VideoCapture>(deviceId);
            if (capture_->isOpened()) {
                if (gc_.isDebugMode()) {
                    std::cout << "Successfully opened camera device " << deviceId << std::endl;
                }
                return;
            }
            
            // If not opened, throw exception
            throw CameraException("Failed to initialize camera device");
        } 
        catch (const cv::Exception& e) {
            if (attempt == maxAttempts) {
                std::stringstream ss;
                ss << "Camera initialization failed (attempt " << attempt << "): " << e.what();
                throw CameraException(ss.str());
            }
            
            if (gc_.isDebugMode()) {
                std::cout << "Camera open attempt " << attempt << " failed: " << e.what() << std::endl;
            }
            
            // Wait before retry
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
    
    throw CameraException("All camera initialization attempts failed for device " + std::to_string(deviceId));
}

CameraCapture::~CameraCapture() {
    stopCapture();
    if (capture_ && capture_->isOpened()) {
        capture_->release();
    }
}

void CameraCapture::startCapture() {
    // No thread needed, just mark as running
    isRunning_ = true;
}

void CameraCapture::stopCapture() {
    isRunning_ = false;
}

void CameraCapture::takeSnapshot() {
    cv::Mat frame;
    if (capture_->read(frame)) {
        std::lock_guard<std::mutex> lock(frameMutex_);
        latestFrame_ = frame.clone();
        
        // Save with timestamp
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()).count();
        std::string filename = "snapshot_" + std::to_string(timestamp) + ".jpg";
        saveLatestFrame(filename);
    }
}

cv::Mat CameraCapture::getLatestFrame() {
    std::lock_guard<std::mutex> lock(frameMutex_);
    return latestFrame_.clone();
}

void CameraCapture::saveLatestFrame(const std::string& filename) {
    cv::Mat frame = getLatestFrame();
    if (frame.empty()) {
        throw CameraException("Cannot save empty frame");
    }
    if (!cv::imwrite(filename, frame)) {
        throw CameraException("Failed to save frame to file: " + filename);
    }
}
