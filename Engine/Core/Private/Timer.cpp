#include "Timer.h"

namespace timer {
    Timer::Timer(): _nextTimerHandle(0),
                    _timers(types::unordered_map_arena<timer_handle, TimerInfo>()) {}

    timer_handle Timer::reserveHandle() {
        return _nextTimerHandle++;
    }

    TimerInfo* Timer::getTimerInfo(timer_handle handle) {
        if (!_timers.contains(handle)) {
            return nullptr;
        }

        return &_timers[handle];
    }

    void Timer::start(timer_handle handle, float durationSeconds, bool shouldLoop, std::function<void()>&& callback) {
        TimerInfo info
        {
            handle,
            durationSeconds,
            shouldLoop,
            std::move(callback),
            0
        };
        _timers[handle] = info;
    }

    timer_handle Timer::start(float durationSeconds, bool shouldLoop, std::function<void()>&& callback) {
        timer_handle handle = reserveHandle();
        start(handle, durationSeconds, shouldLoop, std::move(callback));
        return handle;
    }

    void Timer::delay(float delayDurationSeconds, std::function<void()>&& callback) {
        start(delayDurationSeconds, false, std::move(callback));
    }

    void Timer::tick(float dt) {
        types::set_arena<timer_handle> timersToCancel;
        for (auto& entry: _timers) {
            TimerInfo& info = entry.second;
            info.secondsElapsed += dt;
            if (info.secondsElapsed >= info.durationSeconds) {
                info.callback();

                if (info.shouldLoop) {
                    info.secondsElapsed = 0;
                } else {
                    timersToCancel.insert(info.handle);
                }
            }
        }

        for (timer_handle handle: timersToCancel) {
            cancel(handle);
        }
    }

    void Timer::reset(timer_handle handle) {
        TimerInfo& info = _timers[handle];
        info.secondsElapsed = 0;
    }

    void Timer::cancel(timer_handle handle) {
        _timers.erase(handle);
    }

    void Timer::shutdown() {
        _timers.clear();
    }
}
