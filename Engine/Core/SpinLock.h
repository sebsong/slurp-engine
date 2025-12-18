#pragma once
#include <atomic>

namespace lock {
    class SpinLock {
    public:
        SpinLock();

        bool tryLock();

        void lock();

        void release();

    private:
        std::atomic<bool> _isLocked;
    };
}
