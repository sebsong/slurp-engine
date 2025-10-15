#pragma once

#include "SpinLock.h"

#include <cstdint>
#include <deque>
#include <functional>
#include <thread>

#define WORKER_POOL_SIZE 8

namespace job {
    typedef uint32_t job_id;

    struct Job {
        job_id jobId;
        std::function<void()> fn;
    };

    class JobRunner {
    public:
        JobRunner();

        job_id queueJob(std::function<void()>&& fn);

        // TODO: method for checking status of a job_id
        // TODO: method for waiting on a job_id


    private:
        job_id _nextJobId;
        lock::SpinLock _jobQueueLock;
        std::deque<Job> _jobQueue;
        std::thread _workerPool[WORKER_POOL_SIZE]; // TODO: lock threads to CPU cores

        void _processJobs();
    };
}
