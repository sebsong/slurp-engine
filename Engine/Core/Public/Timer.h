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

    class Timer {
    public:
        Timer();

        timer_handle getNewHandle();

        void start(timer_handle handle, float durationSeconds, bool shouldLoop, std::function<void()>&& callback);

        timer_handle start(float durationSeconds, bool shouldLoop, std::function<void()>&& callback);

        void delay(float delayDurationSeconds, std::function<void()>&& callback);

        void tick(float dt);

        void reset(timer_handle handle);

        void cancel(timer_handle handle);

    private:
        timer_handle _nextTimerHandle;
        // TODO: allocate memory from the shared memory block
        types::unordered_map_arena<timer_handle, TimerInfo> _timers;
    };

    /** Global Methods **/

    timer_handle getNewHandle();

    void start(timer_handle handle, float durationSeconds, bool shouldLoop, std::function<void()>&& callback);

    timer_handle start(float durationSeconds, bool shouldLoop, std::function<void()>&& callback);

    void delay(float delayDurationSeconds, std::function<void()>&& callback);

    void tick(float dt);

    void reset(timer_handle handle);

    void cancel(timer_handle handle);
}
