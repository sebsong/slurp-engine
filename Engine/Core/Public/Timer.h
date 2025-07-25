#pragma once
#include <cstdint>
#include <functional>

namespace timer {
    typedef uint32_t timer_handle;

    struct TimerInfo {
        timer_handle handle;
        float durationSeconds;
        bool shouldLoop;
        std::function<void()> callback;
        float secondsElapsed;
    };

    timer_handle getNewHandle();

    timer_handle start(float durationSeconds, bool shouldLoop, std::function<void()>&& callback);

    void start(timer_handle handle, float durationSeconds, bool shouldLoop, std::function<void()>&& callback);

    void tick(float dt);

    void reset(timer_handle handle);

    void cancel(timer_handle handle);
}
