#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <thread>

namespace job {
    typedef uint32_t job_id;

    struct Job {
        job_id jobId;
        std::function<void()> fn;
    };

    class JobRunner {
    public:
        JobRunner();

        template<typename T>
        job_id queueJob(T fn) {
            Job job{
                _nextJobId++,
                fn
            };

            _jobQueue.push_back(job);

            return job.jobId;
        }

        // TODO: method for checking status of a job_id
        // TODO: method for waiting on a job_id

        void runJobs();

    private:
        job_id _nextJobId;
        std::deque<Job> _jobQueue;
        std::thread _jobThread;
        // TODO: thread pool
    };
}
