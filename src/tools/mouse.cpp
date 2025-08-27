#include "mouse.h"
#include "global_count.h"
#include "common_utils.h"
#include "blue_screen.h"
#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

std::string getTimestampFilename() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".jpg";
    return oss.str();
}

void ensureImgDirectoryExists() {
    fs::path imgDir("img");
    if (!fs::exists(imgDir)) {
        fs::create_directory(imgDir);
    }
}

class MouseHookManager {
public:
    MouseHookManager(GlobalCount* gc) : gc_(gc) {
        screen_width_ = GetSystemMetrics(SM_CXSCREEN);
        screen_height_ = GetSystemMetrics(SM_CYSCREEN);
        center_x_ = screen_width_ / 2;
        center_y_ = screen_height_ / 2;
    }

    ~MouseHookManager() {
        CommonUtils::SafeRelease(hook_);
        CommonUtils::SafeRelease(hWnd_);
    }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        MouseHookManager* pThis = reinterpret_cast<MouseHookManager*>(
            GetWindowLongPtr(hWnd, GWLP_USERDATA));

        if (message == WM_COPYDATA) {
            PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;
            if (pThis && pcds && pcds->lpData) {
                pThis->handleMouseEvent(pcds->dwData, (MSLLHOOKSTRUCT*)pcds->lpData);
            }
            return TRUE;
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    void start() {
        // Create message window using common utility
        hWnd_ = CommonUtils::CreateMessageWindow(L"MouseHookWindowClass", WndProc, this);
        
        hook_ = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, GetModuleHandle(NULL), 0);
        if (!hook_) {
            DestroyWindow(hWnd_);
            throw std::runtime_error("Failed to set mouse hook");
        }

        if (gc_->isDebugMode()) {
            std::cout << "Mouse hook started" << std::endl;
        }
    }

    void processMessages() {
        MSG msg;
        std::cout << "Waiting for messages..." << std::endl;
        while (gc_->shouldUpdateTime()) {
            GetMessage(&msg, NULL, 0, 0);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void lockMouse() {
        if (!SetCursorPos(center_x_, center_y_) && gc_->isDebugMode()) {
            std::cout << "Failed to set cursor position" << std::endl;
        }
    }

private:
    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0) {
            MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
            // Find our hidden window
            HWND hWnd = FindWindowW(L"MouseHookWindowClass", NULL);
            if (hWnd) {
                // Send message to our window with the mouse event data
                COPYDATASTRUCT cds;
                cds.dwData = wParam; // Mouse message type
                cds.cbData = sizeof(MSLLHOOKSTRUCT);
                cds.lpData = pMouseStruct;
                SendMessageW(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);
            }
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    void debugPrint(const std::string& message) const {
        if (gc_->isDebugMode()) {
            std::cout << message << std::endl;
        }
    }

    void handleMouseEvent(WPARAM wParam, MSLLHOOKSTRUCT* pMouseStruct) {
        // Log all mouse events in debug mode
        if (gc_->isDebugMode()) {
            logMouseEvent(wParam, pMouseStruct);
        }

        // Reset timer if mouse is not locked
        if (!gc_->isMouseLocked()) {
            gc_->resetTime();
        }

        // Handle middle button click
        if (wParam == WM_MBUTTONDOWN) {
            gc_->setMouseLock(false);
            debugPrint("Middle button down detected, attempting to unlock mouse...");
            debugPrint("Mouse lock state after unlock attempt: " + 
                     std::string(gc_->isMouseLocked() ? "locked" : "unlocked"));
        }

        // Handle mouse move
        if (wParam == WM_MOUSEMOVE) {
            auto [x, y] = getMouseCoords(pMouseStruct);
            debugPrint("Mouse moved to (" + std::to_string(x) + ", " + std::to_string(y) + ")");
        }
    }

    [[nodiscard]] static std::tuple<int, int> getMouseCoords(MSLLHOOKSTRUCT* pMouseStruct) {
        return std::make_tuple(pMouseStruct->pt.x, pMouseStruct->pt.y);
    }

    void logMouseEvent(WPARAM wParam, MSLLHOOKSTRUCT* pMouseStruct) {
        auto [x, y] = getMouseCoords(pMouseStruct);
        
        std::string_view eventName;
        switch (wParam) {
            case WM_LBUTTONDOWN: eventName = MOUSE_EVENT_NAMES[0]; break;
            case WM_RBUTTONDOWN: eventName = MOUSE_EVENT_NAMES[1]; break;
            case WM_MBUTTONDOWN: eventName = MOUSE_EVENT_NAMES[2]; break;
            case WM_MOUSEMOVE: eventName = MOUSE_EVENT_NAMES[3]; break;
            case WM_MOUSEWHEEL: eventName = MOUSE_EVENT_NAMES[4]; break;
            default: return;
        }

        if constexpr (sizeof(void*) == 8) {
            std::cout << "[64-bit] ";
        } else {
            std::cout << "[32-bit] ";
        }
        std::cout << eventName << " at (" << x << ", " << y << ")\n";
    }

    GlobalCount* gc_;
    HHOOK hook_ = nullptr;
    HWND hWnd_ = nullptr;
    int screen_width_;
    int screen_height_;
    int center_x_;
    int center_y_;
};

void update_click(GlobalCount* gc) {
    try {
        MouseHookManager mouseManager(gc);
        mouseManager.start();

        std::thread mouseLockThread([gc, &mouseManager]() {
            while (gc->shouldUpdateTime()) {
                if (gc->isMouseLocked()) {
                    /*if (gc->isDebugMode()) {
                        std::cout << "Mouse is locked, performing lock operation..." << std::endl;
                    }
                    */
                    mouseManager.lockMouse();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Increased from 10ms to 100ms
            }
        });

        mouseManager.processMessages();

        if (mouseLockThread.joinable()) {
            mouseLockThread.join();
        }

        if (gc->isDebugMode()) {
            std::cout << "Mouse hook stopped" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Critical mouse error: " << e.what() << std::endl;
        // Re-throw to ensure program exits
        throw;
    }
}
