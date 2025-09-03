#include <iostream>
#include <filesystem>
#include "gui.h"
#include "./utils/logger.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QStyle>
#include "../include/core.h"
#include "./core/global_count/global_count.h"
#pragma comment(lib, "user32.lib")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 初始化全局计数器
    GlobalCount gc;
    
    // 启动core线程
    std::atomic<bool> running(true);
    std::thread coreThread([&gc, &running]() {
        try {
            runCoreApplication(gc);
        } catch (...) {
            running = false;
        }
    });

    // 初始化日志系统
    namespace fs = std::filesystem;
    fs::path logDir("logs");
    if (!fs::exists(logDir)) {
        fs::create_directory(logDir);
    }
    Logger::getInstance().init("logs/mouselock.log");

    // 程序退出时停止线程
    QObject::connect(&a, &QApplication::aboutToQuit, [&]() {
        Logger::getInstance().log("正在停止线程...");
        running = false;
        gc.stopUpdates(); // 确保全局计数器停止更新
        
        /*if (coreThread.joinable()) {
            Logger::getInstance().log("等待线程结束，最多等待5秒...");
            auto start = std::chrono::steady_clock::now();
            coreThread.join();
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            Logger::getInstance().log("线程已成功停止，耗时 " + std::to_string(duration) + " 毫秒");
        }
        */
        Logger::getInstance().log("程序退出中...");
        exit(0);
    });

    // 处理异常退出
    std::set_terminate([](){
        Logger::getInstance().log("错误: 发生未捕获异常，强制退出程序");
        std::exit(1);
    });

    Logger::getInstance().log("MouseLock 程序已启动");
    
    // 创建系统托盘图标
    QSystemTrayIcon trayIcon;
    trayIcon.setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    trayIcon.setToolTip("MouseLock");
    
    // 创建右键菜单
    QMenu* trayMenu = new QMenu();
    
    // 添加设置菜单项
    QAction* settingsAction = new QAction("设置", trayMenu);
    QObject::connect(settingsAction, &QAction::triggered, [&]() {
        static qt_std1 w; // 使用static确保窗口只创建一次
        w.show();
    });
    trayMenu->addAction(settingsAction);
    
    // 添加退出菜单项
    QAction* quitAction = new QAction("退出", trayMenu);
    QObject::connect(quitAction, &QAction::triggered, &a, &QApplication::quit);
    trayMenu->addAction(quitAction);
    
    // 设置托盘菜单
    trayIcon.setContextMenu(trayMenu);
    
    // 显示托盘图标
    trayIcon.show();
    
    return a.exec();
}