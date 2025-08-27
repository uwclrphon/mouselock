#include <string>
#include <string_view>
#ifndef MOUSE_H
#define MOUSE_H

inline constexpr std::string_view MOUSE_EVENT_NAMES[] = {
    "Left button", 
    "Right button", 
    "Middle button",
    "Mouse move",
    "Mouse wheel"
};

struct GlobalCount;

void log_mouse_event(std::string_view event_name, int x, int y);

void update_click(GlobalCount *gc);

#endif