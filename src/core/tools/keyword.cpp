#include "../../../include/keyword.h"
#include "../global_count/global_count.h"
#include "../../../include/common_utils.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <optional>
#include <string_view>

class KeyboardHookManager {
public:
    KeyboardHookManager(GlobalCount* gc) : gc_(gc) {}

    ~KeyboardHookManager() {
        CommonUtils::SafeRelease(hook_);
        CommonUtils::SafeRelease(hWnd_);
    }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        KeyboardHookManager* pThis = reinterpret_cast<KeyboardHookManager*>(
            GetWindowLongPtr(hWnd, GWLP_USERDATA));

        if (message == WM_COPYDATA) {
            PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;
            if (pThis && pcds && pcds->lpData) {
                pThis->handleKeyEvent(pcds->dwData, (KBDLLHOOKSTRUCT*)pcds->lpData);
            }
            return TRUE;
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    void start() {
        // Create message window using common utility
        hWnd_ = CommonUtils::CreateMessageWindow(L"KeyboardHookWindowClass", WndProc, this);
        
        hook_ = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);
        if (!hook_) {
            DestroyWindow(hWnd_);
            throw std::runtime_error("Failed to set keyboard hook");
        }

        if (gc_->isDebugMode()) {
            std::cout << "Keyboard hook started" << std::endl;
        }
    }

    void processMessages() {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) && gc_->shouldUpdateTime()) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

private:
    static LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= 0) {
            KBDLLHOOKSTRUCT* pKeyboardStruct = (KBDLLHOOKSTRUCT*)lParam;
            // Find our hidden window
            HWND hWnd = FindWindowW(L"KeyboardHookWindowClass", NULL);
            if (hWnd) {
                // Send message to our window with the keyboard event data
                COPYDATASTRUCT cds;
                cds.dwData = wParam; // Keyboard message type
                cds.cbData = sizeof(KBDLLHOOKSTRUCT);
                cds.lpData = pKeyboardStruct;
                SendMessageW(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);
            }
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    [[nodiscard]] static std::optional<std::wstring> getKeyName(KBDLLHOOKSTRUCT* pKeyboardStruct) {
        wchar_t keyName[32];
        if (GetKeyNameTextW(pKeyboardStruct->scanCode << 16, keyName, sizeof(keyName)/sizeof(wchar_t))) {
            return keyName;
        }
        return std::nullopt;
    }

    void handleKeyEvent(WPARAM wParam, KBDLLHOOKSTRUCT* pKeyboardStruct) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            if (gc_->isDebugMode()) {
                if (auto keyName = getKeyName(pKeyboardStruct)) {
                    std::wcout << L"Key pressed: " << *keyName << std::endl;
                }
            }
            
            if(!gc_->isMouseLocked()) {
                gc_->resetTime();
            }
        }
    }

    GlobalCount* gc_;
    HHOOK hook_ = nullptr;
    HWND hWnd_ = nullptr;
};

void update_keyword(GlobalCount* gc) {
    try {
        KeyboardHookManager keyboardManager(gc);
        keyboardManager.start();
        keyboardManager.processMessages();

        if (gc->isDebugMode()) {
            std::cout << "Keyboard hook stopped" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Keyboard error: " << e.what() << std::endl;
    }
}
