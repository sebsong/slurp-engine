#include "SpinLock.h"

namespace lock {
    SpinLock::SpinLock(): _isLocked(false) {}

    bool SpinLock::tryLock() {
        bool expected = false;
        bool desired = true;
        return _isLocked.compare_exchange_weak(
            expected,
            desired,
            std::memory_order_acquire,
            std::memory_order_relaxed
        );
    }

    void SpinLock::lock() {
        while (!tryLock()) {}
    }

    void SpinLock::release() {
        // TODO: technically shouldn't release the lock unless you own it
        _isLocked.store(false, std::memory_order_release);
    }
}
