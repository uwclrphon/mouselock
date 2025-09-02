#include <windows.h>
#include <memory>
#include <thread>
#include <iostream>
#include "../utils/logger.h"
#include "global_count/global_count.h"
#include "../include/times.h"
#include "../include/mouse.h"
#include "../include/keyword.h"
#include "blue_screen/blue_screen.h"
#include "../include/json_utils/json_utils.h"
#include "../include/core.h"

ThreadPool::ThreadPool(GlobalCount& gc) : gc_(gc) {
    startThreads();
}

ThreadPool::~ThreadPool() {
    stopThreads();
}

void ThreadPool::startThreads() {
    time_thread_ = std::thread(update_time, &gc_);
    click_thread_ = std::thread(update_click, &gc_);
    keyword_thread_ = std::thread(update_keyword, &gc_);
}

void ThreadPool::stopThreads() {
    Logger::getInstance().log("开始停止ThreadPool所有子线程...");
    gc_.stopUpdates();
    
    auto joinThread = [](const std::string& name, std::thread& t) {
        if (t.joinable()) {
            Logger::getInstance().log("等待" + name + "线程停止...");
            auto start = std::chrono::steady_clock::now();
            t.join();
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            Logger::getInstance().log(name + "线程已停止，耗时: " + std::to_string(duration) + "ms");
        } else {
            Logger::getInstance().log(name + "线程已停止或不可连接");
        }
    };
    
    joinThread("时间更新", time_thread_);
    joinThread("鼠标点击", click_thread_); 
    joinThread("键盘监听", keyword_thread_);
    
    Logger::getInstance().log("ThreadPool所有子线程已停止");
}

void runCoreApplication(GlobalCount& gc) {
    try {
        #ifdef ENABLE_DEBUG_MODE
        Logger::getInstance().log("进入调试模式，控制台窗口可见");
        std::cout << "Debug mode - Console window visible" << std::endl;
        #else
        Logger::getInstance().log("进入生产模式，隐藏控制台");
        FreeConsole();
        SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
        #endif

        Logger::getInstance().log("启动蓝屏模拟线程...");
        BlueScreenSimulator blueScreen(gc);
        std::thread blueScreenThread([&gc, &blueScreen]() {
            Logger::getInstance().log("蓝屏模拟线程运行中");
            try {
                while (gc.shouldUpdateTime()) {
                    if (gc.isFakeBlueScreen() && !gc.isBlueScreenActive()) {
                        Logger::getInstance().log("启动蓝屏模拟");
                        blueScreen.start();
                    } else if (!gc.isFakeBlueScreen() && gc.isBlueScreenActive()) {
                        Logger::getInstance().log("停止蓝屏模拟");
                        blueScreen.stop();
                    }
                    // 更频繁地检查停止条件
                    for (int i = 0; i < 5 && gc.shouldUpdateTime(); ++i) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    }
                }
                Logger::getInstance().log("蓝屏模拟线程正常退出");
            } catch (const std::exception& e) {
                std::cerr << "蓝屏线程错误: " << e.what() << std::endl;
                Logger::getInstance().log(std::string("蓝屏线程错误: ") + e.what());
            }
        });

        ThreadPool pool(gc);
        
        if (blueScreenThread.joinable()) {
            Logger::getInstance().log("等待蓝屏模拟线程停止...");
            auto start = std::chrono::steady_clock::now();
            blueScreenThread.join();
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            Logger::getInstance().log("蓝屏模拟线程已停止，耗时: " + std::to_string(duration) + "ms");
        }
    } catch (const std::exception& e) {
        // Error handling
    }
}
