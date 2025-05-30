#pragma once
#include <cstdint>
#include <functional>

namespace timer
{
    typedef uint32_t timer_handle;

    struct TimerInfo
    {
        timer_handle handle;
        float duration;
        bool shouldLoop;
        std::function<void()> callback;
        float timer;
    };
    
    timer_handle registerTimer(float duration, bool shouldLoop, std::function<void()> callback);
    
    void tick(float dt);
    
    void cancelTimer(timer_handle handle);
}
