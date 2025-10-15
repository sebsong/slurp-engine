#include "JobRunner.h"

namespace job {
    JobRunner::JobRunner(): _nextJobId(0),
                            _jobQueue(std::deque<Job>()) {
        for (int i = 0; i < WORKER_POOL_SIZE; i++) {
            _workerPool[i] = std::thread(&JobRunner::_processJobs, this);
        }
    }

    void JobRunner::_processJobs() {
        while (true) {
            if (_jobQueue.empty()) {
                continue;
            }
            // TODO: need to lock

            Job job = _jobQueue.front();
            _jobQueue.pop_front();
            job.fn();
        }
    }
}
