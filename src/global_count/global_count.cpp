#include "global_count.h"
#include <sstream>

GlobalCount::GlobalCount() {}

GlobalCount::~GlobalCount() {}

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
