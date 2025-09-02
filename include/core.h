#pragma once
#include <memory>
#include <thread>
#include "../src/core/global_count/global_count.h"
#include "../src/core/blue_screen/blue_screen.h"

class ThreadPool {
public:
    ThreadPool(GlobalCount& gc);
    ~ThreadPool();

private:
    void startThreads();
    void stopThreads();

    GlobalCount& gc_;
    std::thread time_thread_;
    std::thread click_thread_;
    std::thread keyword_thread_;
};

// Core application functionality
void runCoreApplication(GlobalCount& gc);
