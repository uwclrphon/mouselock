#include "../global_count/global_count.h"
#include <iostream>
#include <chrono>
#include <thread>
#include "../../../include/times.h"

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
        if (gc->getTime() > gc->getMaxMouseLockTime()  && !gc->isMouseLocked()) {
            gc->setMouseLock(true);
        }
    }

    if (gc->isDebugMode()) {
        std::cout << "Stopping time update thread" << std::endl;
    }
}
