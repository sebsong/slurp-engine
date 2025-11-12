#pragma once
#include "CollectionTypes.h"

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

        timer_handle reserveHandle();

        TimerInfo* getTimerInfo(timer_handle handle);

        void start(timer_handle handle, float durationSeconds, bool shouldLoop, std::function<void()>&& callback);

        timer_handle start(float durationSeconds, bool shouldLoop, std::function<void()>&& callback);

        void delay(float delayDurationSeconds, std::function<void()>&& callback);

        void tick(float dt);

        void reset(timer_handle handle);

        void cancel(timer_handle handle);

        void shutdown();

    private:
        timer_handle _nextTimerHandle;
        types::unordered_map_arena<timer_handle, TimerInfo> _timers;
    };

    /** Global Methods **/

    inline timer_handle reserveHandle() {
        return slurp::Globals->Timer->reserveHandle();
    }

    inline TimerInfo* getTimerInfo(timer_handle handle) {
        return slurp::Globals->Timer->getTimerInfo(handle);
    }

    inline void start(timer_handle handle, float durationSeconds, bool shouldLoop, std::function<void()>&& callback) {
        return slurp::Globals->Timer->start(handle, durationSeconds, shouldLoop, std::move(callback));
    }

    inline timer_handle start(float durationSeconds, bool shouldLoop, std::function<void()>&& callback) {
        return slurp::Globals->Timer->start(durationSeconds, shouldLoop, std::move(callback));
    }

    inline void delay(float delayDurationSeconds, std::function<void()>&& callback) {
        slurp::Globals->Timer->delay(delayDurationSeconds, std::move(callback));
    }

    inline void tick(float dt) {
        slurp::Globals->Timer->tick(dt);
    }

    inline void reset(timer_handle handle) {
        slurp::Globals->Timer->reset(handle);
    }

    inline void cancel(timer_handle handle) {
        slurp::Globals->Timer->cancel(handle);
    }

    inline void shutdown() {
        slurp::Globals->Timer->shutdown();
    }
}
