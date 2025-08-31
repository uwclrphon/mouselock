#ifndef BLUE_SCREEN_H
#define BLUE_SCREEN_H

#include <windows.h>
#include <atomic>
#include <thread>
#include "global_count.h"

class BlueScreenSimulator {
public:
    explicit BlueScreenSimulator(GlobalCount& gc);
    ~BlueScreenSimulator();

    void start();  // 启动蓝屏模拟
    void stop();   // 停止蓝屏模拟
    bool isActive() const;  // 检查是否正在模拟

private:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void messageLoop();

    HWND blueScreenWindow = nullptr;
    HBRUSH blueBrush = nullptr;
    HBITMAP blueImage = nullptr;
    ULONG_PTR gdiplusToken = 0;  // GDI+ token
    bool isWindowClassRegistered = false;
    std::atomic<bool> shouldStop{false};
    GlobalCount& gc_;
};

#endif // BLUE_SCREEN_H
