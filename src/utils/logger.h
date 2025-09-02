#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void init(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile_.is_open()) {
            logFile_.close();
        }
        logFile_.open(filename, std::ios::app);
    }

    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile_.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto now_time = std::chrono::system_clock::to_time_t(now);
            logFile_ << "[" << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << "] " 
                    << message << std::endl;
        }
    }

private:
    Logger() = default;
    ~Logger() {
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }

    std::ofstream logFile_;
    std::mutex mutex_;
};

#endif // LOGGER_H
