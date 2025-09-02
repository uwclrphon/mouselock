#ifndef GLOBAL_COUNT_H
#define GLOBAL_COUNT_H

#include <atomic>
#include <string>
#include <windows.h>
#include <mutex>
#include <variant>
#include <map>
#include "../../../include/json_utils/json_utils.h"

#ifdef _WIN32
    #ifdef BUILDING_GLOBAL_COUNT_DLL
        #define GLOBAL_COUNT_API __declspec(dllexport)
    #else
        #define GLOBAL_COUNT_API __declspec(dllimport)
    #endif
#else
    #define GLOBAL_COUNT_API
#endif

// 前向声明替代包含头文件
class BlueScreenSimulator;

class GLOBAL_COUNT_API GlobalCount : public DebugLogger {
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
    static constexpr const char* CONFIG_FILE = "global_config.json";
    
    void loadConfig();
    void createDefaultConfig();
    int getConfigValue(const std::string& key, int defaultValue) const;
    
    mutable std::mutex mutex_;
    std::atomic<int> time_{0};
    std::atomic<bool> is_mouse_lock_{false};
    std::atomic<bool> is_update_time_{true};
    int max_mouse_lock_time_;
#ifdef ENABLE_DEBUG_MODE
    const bool debug_mode_{true};
#else
    const bool debug_mode_{false};
#endif
    int fake_blue_screen_time_;
    std::atomic<bool> is_fake_blue_screen_{false};
    std::atomic<bool> is_blue_screen_active_{false};
    std::map<std::string, std::string> config_;

public:
    [[nodiscard]] bool isBlueScreenActive() const {
        return is_blue_screen_active_.load();
    }

    void setBlueScreenActive(bool active) {
        is_blue_screen_active_.store(active);
    }

    [[nodiscard]] int getMaxMouseLockTime() const {
        std::scoped_lock lock(mutex_);
        return max_mouse_lock_time_;
    }

    void setMaxMouseLockTime(int time) {
        std::scoped_lock lock(mutex_);
        max_mouse_lock_time_ = time;
    }
};

#endif
