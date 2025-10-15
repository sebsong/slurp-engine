#include "JobRunner.h"
#include "Logging.h"

#include <format>

namespace job {
    JobRunner::JobRunner(): _nextJobId(0),
                            _jobQueue(std::deque<Job>()) {
        for (int i = 0; i < WORKER_POOL_SIZE; i++) {
            _workerPool[i] = std::thread(&JobRunner::_processJobs, this, i);
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

    void JobRunner::_processJobs(uint8_t workerIndex) {
        logging::info(std::format("Job worker {} started", workerIndex));
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
