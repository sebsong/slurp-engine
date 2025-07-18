﻿#include "Timer.h"
#include <set>

namespace timer {
    // TODO: allocate memory from the shared memory block
    static std::unordered_map<timer_handle, TimerInfo> GlobalTimerMap;
    static timer_handle GlobalNextTimerHandle;

    void delay(float delayDurationSeconds, std::function<void()>&& callback) {
        registerTimer(delayDurationSeconds, false, std::move(callback));
    }

    timer_handle registerTimer(float durationSeconds, bool shouldLoop, std::function<void()>&& callback) {
        timer_handle handle = GlobalNextTimerHandle++;
        TimerInfo info
        {
            handle,
            durationSeconds,
            shouldLoop,
            std::move(callback),
            0
        };
        GlobalTimerMap[handle] = info;
        return handle;
    }

    void tick(float dt) {
        std::set<timer_handle> timersToCancel;
        for (auto& entry: GlobalTimerMap) {
            TimerInfo& info = entry.second;
            info.timer += dt;
            if (info.timer >= info.durationSeconds) {
                info.callback();

                if (info.shouldLoop) { info.timer = 0; }
                else { timersToCancel.insert(info.handle); }
            }
        }

        for (timer_handle handle: timersToCancel) { cancelTimer(handle); }
    }

    void cancelTimer(timer_handle handle) { GlobalTimerMap.erase(handle); }
}
