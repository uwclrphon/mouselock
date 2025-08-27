#include "global_count.h"
#include <iostream>
#include <chrono>
#include <thread>
#include "times.h"

void update_time(GlobalCount *gc) {
    if (gc->isDebugMode()) {
        std::cout << "Starting time update thread" << std::endl;
    }

    while (gc->shouldUpdateTime()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        gc->incrementTime();

        if (gc->isDebugMode()) {
            std::cout << "Time: " << gc->getTime() << std::endl;
        }
        #ifdef ENABLE_DEBUG_MODE
            if (gc->getTime() > 10 && !gc->isMouseLocked()) {
                gc->setMouseLock(true);
            }
        #else
        if (gc->getTime() > 60 && !gc->isMouseLocked()) {
                gc->setMouseLock(true);
            }
        #endif
    }

    if (gc->isDebugMode()) {
        std::cout << "Stopping time update thread" << std::endl;
    }
}
