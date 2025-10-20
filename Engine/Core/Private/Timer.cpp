#include "Timer.h"
#include <set>

namespace timer {
    // TODO: allocate memory from the shared memory block
    static std::unordered_map<timer_handle, TimerInfo> GlobalTimerMap;
    static timer_handle GlobalNextTimerHandle;

    void delay(float delayDurationSeconds, std::function<void()>&& callback) {
        start(delayDurationSeconds, false, std::move(callback));
    }

    timer_handle getNewHandle() {
        return GlobalNextTimerHandle++;
    }

    timer_handle start(float durationSeconds, bool shouldLoop, std::function<void()>&& callback) {
        timer_handle handle = getNewHandle();
        start(handle, durationSeconds, shouldLoop, std::move(callback));
        return handle;
    }

    void start(timer_handle handle, float durationSeconds, bool shouldLoop, std::function<void()>&& callback) {
        TimerInfo info
        {
            handle,
            durationSeconds,
            shouldLoop,
            std::move(callback),
            0
        };
        GlobalTimerMap[handle] = info;
    }

    void tick(float dt) {
        types::set_arena<timer_handle> timersToCancel;
        for (auto& entry: GlobalTimerMap) {
            TimerInfo& info = entry.second;
            info.secondsElapsed += dt;
            if (info.secondsElapsed >= info.durationSeconds) {
                info.callback();

                if (info.shouldLoop) { info.secondsElapsed = 0; }
                else { timersToCancel.insert(info.handle); }
            }
        }

        for (timer_handle handle: timersToCancel) { cancel(handle); }
    }

    void reset(timer_handle handle) {
        TimerInfo& info = GlobalTimerMap[handle];
        info.secondsElapsed = 0;
    }

    void cancel(timer_handle handle) { GlobalTimerMap.erase(handle); }
}
