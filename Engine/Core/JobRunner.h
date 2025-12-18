#pragma once

#include "CollectionTypes.h"
#include "SpinLock.h"

#include <cstdint>
#include <deque>
#include <functional>
#include <thread>

#define WORKER_POOL_SIZE 4

namespace job {
    typedef uint32_t job_id;

    struct Job {
        job_id jobId;
        std::function<void()> fn;
    };

    class JobRunner {
    public:
        JobRunner();

        void initialize();

        job_id queueJob(std::function<void()>&& fn);

        // TODO: method for checking status of a job_id
        // TODO: method for waiting on a job_id

        void shutdown();

    private:
        job_id _nextJobId;
        lock::SpinLock _jobQueueLock;
        types::deque_arena<Job> _jobQueue;
        std::atomic<bool> _killWorkers;
        std::thread _workerPool[WORKER_POOL_SIZE]; // TODO: could set thread CPU core affinity

        void _processJobs(uint8_t workerIndex);
    };

    static void initialize();

    static job_id queueJob(std::function<void()>&& fn);

    static void shutdown();
}
