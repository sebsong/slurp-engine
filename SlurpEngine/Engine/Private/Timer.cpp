#include <Timer.hpp>

namespace timer
{
    // TODO: allocate memory from the shared memory block
    static std::unordered_map<timer_handle, TimerInfo> GlobalTimerMap;
    static timer_handle GlobalNextTimerHandle;

    timer_handle registerTimer(float duration, bool shouldLoop, std::function<void()>&& callback)
    {
        timer_handle handle = GlobalNextTimerHandle++;
        TimerInfo info
        {
            handle,
            duration,
            shouldLoop,
            std::move(callback),
            0
        };
        GlobalTimerMap[handle] = info;
        return handle;
    }

    void tick(float dt)
    {
        for (auto& entry : GlobalTimerMap)
        {
            TimerInfo& info = entry.second;
            info.timer += dt;
            if (info.timer >= info.duration)
            {
                info.callback();
            }
        }
    }

    void cancelTimer(timer_handle handle)
    {
        GlobalTimerMap.erase(handle);
    }
}
