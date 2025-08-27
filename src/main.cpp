#include <iostream>
#include <memory>
#include <thread>
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
    std::cout << "Mouse Lock started" << std::endl;
    try {
        GlobalCount gc;
        BlueScreenSimulator blueScreen(gc);
        gc.setBlueScreen(&blueScreen);
        
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
        
        std::string input;
        std::cin >> input; // Wait for user input to exit
        
        if (blueScreenThread.joinable()) {
            blueScreenThread.join();
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}