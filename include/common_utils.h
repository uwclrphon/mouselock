#pragma once
#include <windows.h>
#include <string>
#include <stdexcept>

class Win32Exception : public std::runtime_error {
public:
    Win32Exception(const std::string& msg, DWORD errorCode = GetLastError())
        : std::runtime_error(msg + " (Error: " + std::to_string(errorCode) + ")"),
          errorCode_(errorCode) {}

    DWORD getErrorCode() const { return errorCode_; }

private:
    DWORD errorCode_;
};

namespace CommonUtils {
    inline HWND CreateMessageWindow(
        LPCWSTR className, 
        WNDPROC wndProc,
        void* userData = nullptr) 
    {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = wndProc;
        wc.hInstance = GetModuleHandleW(NULL);
        wc.lpszClassName = className;

        if (!RegisterClassW(&wc)) {
            throw Win32Exception("Failed to register window class");
        }

        HWND hWnd = CreateWindowExW(
            0, className, L"", 
            0, 0, 0, 0, 0, 
            HWND_MESSAGE, NULL, 
            GetModuleHandleW(NULL), NULL);

        if (!hWnd) {
            throw Win32Exception("Failed to create message window");
        }

        if (userData) {
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(userData));
        }

        return hWnd;
    }

    inline void SafeRelease(HHOOK& hook) {
        if (hook) {
            UnhookWindowsHookEx(hook);
            hook = nullptr;
        }
    }

    inline void SafeRelease(HWND& hWnd) {
        if (hWnd) {
            DestroyWindow(hWnd);
            hWnd = nullptr;
        }
    }
}
