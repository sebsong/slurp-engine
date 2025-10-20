#include "Timer.h"
#include <set>

namespace timer {
    Timer::Timer(): _nextTimerHandle(0),
                    _timers(types::unordered_map_arena<timer_handle, TimerInfo>()) {}

    timer_handle Timer::getNewHandle() {
        return _nextTimerHandle++;
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
        timer_handle handle = getNewHandle();
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

    /** Global Methods **/

    timer_handle getNewHandle() {
        return slurp::GlobalTimer->getNewHandle();
    }

    void start(timer_handle handle, float durationSeconds, bool shouldLoop, std::function<void()>&& callback) {
        return slurp::GlobalTimer->start(handle, durationSeconds, shouldLoop, std::move(callback));
    }

    timer_handle start(float durationSeconds, bool shouldLoop, std::function<void()>&& callback) {
        return slurp::GlobalTimer->start(durationSeconds, shouldLoop, std::move(callback));
    }

    void delay(float delayDurationSeconds, std::function<void()>&& callback) {
        slurp::GlobalTimer->delay(delayDurationSeconds, std::move(callback));
    }

    void tick(float dt) {
        slurp::GlobalTimer->tick(dt);
    }

    void reset(timer_handle handle) {
        slurp::GlobalTimer->reset(handle);
    }

    void cancel(timer_handle handle) {
        slurp::GlobalTimer->cancel(handle);
    }
}
