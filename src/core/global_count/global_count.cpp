#include "global_count.h"
#include "../../../include/json_utils/json_utils.h"
#include <sstream>
#include <fstream>

GlobalCount::GlobalCount() {
    loadConfig();
    max_mouse_lock_time_ = getConfigValue("mouse_lock_time", isDebugMode() ? 10 : 60);
    fake_blue_screen_time_ = getConfigValue("blue_screen_time", isDebugMode() ? 20 : 120);
}

GlobalCount::~GlobalCount() {}

void GlobalCount::loadConfig() {
    JsonManager jsonManager(static_cast<DebugLogger*>(this));
    try {
        config_ = jsonManager.readJson(CONFIG_FILE);
    } catch (...) {
        createDefaultConfig();
    }
}

void GlobalCount::createDefaultConfig() {
    JsonManager jsonManager(static_cast<DebugLogger*>(this));
    std::map<std::string, std::string> defaultConfig = {
        {"mouse_lock_time", isDebugMode() ? "10" : "60"},
        {"blue_screen_time", isDebugMode() ? "20" : "120"}
    };
    jsonManager.writeJson(CONFIG_FILE, defaultConfig);
    config_ = defaultConfig;
}

int GlobalCount::getConfigValue(const std::string& key, int defaultValue) const {
    auto it = config_.find(key);
    if (it != config_.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

void GlobalCount::resetTime() {
    time_.store(0);
}

int GlobalCount::getTime() const {
    return time_.load();
}

void GlobalCount::incrementTime() {
    int newTime = ++time_;
    
    // 更新蓝屏状态标志
    if (newTime > fake_blue_screen_time_) {
        std::scoped_lock lock(mutex_);
        is_fake_blue_screen_.store(true);
    } else {
        std::scoped_lock lock(mutex_);
        is_fake_blue_screen_.store(false);
    }
}

bool GlobalCount::isMouseLocked() const {
    return is_mouse_lock_.load();
}

bool GlobalCount::shouldUpdateTime() const {
    return is_update_time_.load();
}

bool GlobalCount::isDebugMode() const {
    return debug_mode_;
}

int GlobalCount::getFakeBlueScreenTime() const {
    return fake_blue_screen_time_;
}

bool GlobalCount::isFakeBlueScreen() const {
    return is_fake_blue_screen_.load();
}

GlobalCount::DebugInfo GlobalCount::getDebugInfo() const {
    std::scoped_lock lock(mutex_);
    std::stringstream ss;
    ss << "Time: " << time_.load() 
       << ", MouseLock: " << is_mouse_lock_.load()
       << ", FakeBlueScreen: " << is_fake_blue_screen_.load();
    
    return ss.str();
}
