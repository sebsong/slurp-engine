#include "JobRunner.h"
#include "Logging.h"

#include <format>

namespace job {
    JobRunner::JobRunner(): _nextJobId(0),
                            _jobQueue(types::deque_arena<Job>()) {
        _killWorkers.store(false);
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

    void JobRunner::shutdown() {
        _killWorkers.store(true);
        for (std::thread& worker: _workerPool) {
            worker.join();
        }
    }

    void JobRunner::_processJobs(uint8_t workerIndex) {
        logging::info(std::format("Job worker {} started", workerIndex));
        while (true) {
            if (_killWorkers.load()) {
                return;
            }

            if (_jobQueue.empty()) {
                // TODO: put these threads to sleep if there are no jobs for many iterations to save CPU
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
