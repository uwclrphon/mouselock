#include "blue_screen.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <iostream>
#include <atomic>

// 确保PROPID类型定义可用
#ifndef PROPID
#define PROPID unsigned long
#endif

LRESULT CALLBACK BlueScreenSimulator::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    BlueScreenSimulator* pThis = reinterpret_cast<BlueScreenSimulator*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    
    switch (message) {
        case WM_PAINT: {
            if (pThis) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                
                if (pThis->blueImage) {
                    Gdiplus::Graphics graphics(hdc);
                    Gdiplus::Bitmap bitmap(pThis->blueImage, nullptr);
                    
                    // 设置高质量插值模式
                    graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                    
                    // 绘制到整个窗口
                    graphics.DrawImage(&bitmap, 
                        Gdiplus::Rect(0, 0, 
                            GetSystemMetrics(SM_CXSCREEN), 
                            GetSystemMetrics(SM_CYSCREEN)),
                        0, 0, bitmap.GetWidth(), bitmap.GetHeight(),
                        Gdiplus::UnitPixel);
                }
                
                EndPaint(hWnd, &ps);
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            if (wParam == VK_LWIN || wParam == VK_RWIN) {
                break;
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            return 0;
        case WM_MBUTTONDOWN:
            if (pThis) {
                pThis->stop();
            }
            return 0;
        case WM_INPUT:
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

BlueScreenSimulator::BlueScreenSimulator(GlobalCount& gc) : gc_(gc) {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
}

BlueScreenSimulator::~BlueScreenSimulator() {
    stop();
    if (blueBrush) {
        DeleteObject(blueBrush);
    }
    if (blueImage) {
        DeleteObject(blueImage);
    }
    Gdiplus::GdiplusShutdown(gdiplusToken);
}

void BlueScreenSimulator::start() {
    if (blueScreenWindow) {
        std::cout << "Blue screen window already exists" << std::endl;
        return;
    }
    
    gc_.setBlueScreenActive(true);

    // 加载图片
    Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(L"blue.png");
    if (bitmap) {
        bitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255), &blueImage);
        delete bitmap;
    } else {
        std::cerr << "Failed to load blue.png" << std::endl;
        return;
    }

    // 注册窗口类
    WNDCLASSW wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"BlueScreenWindowClass";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    
    if (!RegisterClassW(&wc)) {
        std::cerr << "Failed to register window class: " << GetLastError() << std::endl;
        return;
    }

    // 创建全屏窗口
    blueScreenWindow = CreateWindowExW(
        WS_EX_TOPMOST,
        L"BlueScreenWindowClass", 
        nullptr,
        WS_POPUP | WS_VISIBLE,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr
    );

    if (!blueScreenWindow) {
        MessageBoxW(nullptr, L"Failed to create window", L"Error", MB_ICONERROR);
        return;
    }

    // 存储this指针到窗口额外数据
    SetWindowLongPtr(blueScreenWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    // 强制置顶并显示窗口
    SetWindowPos(blueScreenWindow, HWND_TOPMOST, 0, 0, 0, 0, 
                SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    UpdateWindow(blueScreenWindow);
    
    // 激活窗口并设置焦点
    SetForegroundWindow(blueScreenWindow);
    SetFocus(blueScreenWindow);
    
    // 发送空消息激活消息队列
    PostMessage(blueScreenWindow, WM_NULL, 0, 0);

    // 检查图片是否加载成功
    if (!blueImage) {
        MessageBoxW(blueScreenWindow, L"Failed to load blue.png\nPlease make sure the file exists in the same directory", L"Error", MB_ICONERROR);
        stop();
        return;
    }

    messageLoop();
}

void BlueScreenSimulator::stop() {
    if (blueScreenWindow) {
        DestroyWindow(blueScreenWindow);
        blueScreenWindow = nullptr;
    }
    gc_.setBlueScreenActive(false);
}

bool BlueScreenSimulator::isActive() const {
    return blueScreenWindow != nullptr;
}

void BlueScreenSimulator::messageLoop() {
    MSG msg;
    while (isActive() && GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_MBUTTONDOWN) {
            OutputDebugStringA("WM_MBUTTONDOWN received\n");
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // Unlock mouse when message loop ends
    gc_.setMouseLock(false);
}
