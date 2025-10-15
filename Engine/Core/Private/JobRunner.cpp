#include "JobRunner.h"

namespace job {
    JobRunner::JobRunner(): _nextJobId(0), _jobQueue(std::deque<Job>()), _jobThread(std::thread(&JobRunner::runJobs, this)) {}

    void JobRunner::runJobs() {
        while (true) {
            if (_jobQueue.empty()) {
                continue;
            }

            Job job = _jobQueue.front();
            _jobQueue.pop_front();
            job.fn();
        }
    }
}
