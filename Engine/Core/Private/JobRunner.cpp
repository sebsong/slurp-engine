#include "JobRunner.h"

namespace job {
    JobRunner::JobRunner(): _nextJobId(0),
                            _jobQueue(std::deque<Job>()) {
        for (int i = 0; i < WORKER_POOL_SIZE; i++) {
            _workerPool[i] = std::thread(&JobRunner::_processJobs, this);
        }
    }

    job_id JobRunner::queueJob(std::function<void()>&& fn) {
        Job job{
            _nextJobId++,
            std::move(fn)
        };

        _jobQueue.push_back(job);

        return job.jobId;
    }

    void JobRunner::_processJobs() {
        while (true) {
            if (_jobQueue.empty()) {
                continue;
            }

            _jobQueueLock.lock();
            if (_jobQueue.empty()) {
                _jobQueueLock.release();
                continue;
            }
            Job job = _jobQueue.front();
            _jobQueue.pop_front();
            _jobQueueLock.release();

            job.fn();
        }
    }
}
