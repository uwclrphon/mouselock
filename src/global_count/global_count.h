#ifndef GLOBAL_COUNT_H
#define GLOBAL_COUNT_H

#include <atomic>
#include <string>
#include <windows.h>
#include <mutex>
#include <variant>

// 前向声明替代包含头文件
class BlueScreenSimulator;
#ifdef ENABLE_DEBUG_MODE
    inline constexpr int DEFAULT_BLUE_SCREEN_TIME = 20;
#else 
    inline constexpr int DEFAULT_BLUE_SCREEN_TIME = 120;
#endif
inline constexpr int DEFAULT_MOUSE_LOCK_TIME = 10;

class GlobalCount {
public:
    using DebugInfo = std::variant<int, std::string, bool>;
    
    GlobalCount();
    ~GlobalCount();
    
    void resetTime();
    [[nodiscard]] int getTime() const;
    void incrementTime();
    
    [[nodiscard]] bool isMouseLocked() const;
    void setMouseLock(bool locked) {
        std::scoped_lock lock(mutex_);
        is_mouse_lock_.store(locked);
    }
    
    [[nodiscard]] bool shouldUpdateTime() const;
    void stopUpdates() {
        std::scoped_lock lock(mutex_);
        is_update_time_.store(false);
    }
    
    [[nodiscard]] bool isDebugMode() const;
    
    [[nodiscard]] int getFakeBlueScreenTime() const;
    [[nodiscard]] bool isFakeBlueScreen() const;
    void setFakeBlueScreen(bool enabled) {
        std::scoped_lock lock(mutex_);
        is_fake_blue_screen_.store(enabled);
    }
    
    [[nodiscard]] DebugInfo getDebugInfo() const;
    
private:
    mutable std::mutex mutex_;
    std::atomic<int> time_{0};
    std::atomic<bool> is_mouse_lock_{false};
    std::atomic<bool> is_update_time_{true};
    const int max_mouse_lock_time_{DEFAULT_MOUSE_LOCK_TIME};
#ifdef ENABLE_DEBUG_MODE
    const bool debug_mode_{true};
#else
    const bool debug_mode_{false};
#endif
    const int fake_blue_screen_time_{DEFAULT_BLUE_SCREEN_TIME};
    std::atomic<bool> is_fake_blue_screen_{false};
    std::atomic<bool> is_blue_screen_active_{false};

public:
    [[nodiscard]] bool isBlueScreenActive() const {
        return is_blue_screen_active_.load();
    }

    void setBlueScreenActive(bool active) {
        is_blue_screen_active_.store(active);
    }

private:
};

#endif
