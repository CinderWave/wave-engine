#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstdint>

namespace wave::engine::core::jobs {

// Simple job type: fire-and-forget function.
using Job = std::function<void()>;

// Lightweight handle to wait for a group of jobs.
class JobHandle {
public:
    JobHandle() = default;

    explicit JobHandle(std::shared_ptr<std::atomic<std::uint32_t>> counter)
        : m_counter(std::move(counter)) {}

    // Blocks until all associated jobs are complete.
    void wait() const;

    bool valid() const { return static_cast<bool>(m_counter); }

private:
    std::shared_ptr<std::atomic<std::uint32_t>> m_counter;
};

// Job system with a fixed pool of worker threads.
// Intended as an engine-level service, owned by the runtime.
class JobSystem final {
public:
    JobSystem() = default;
    ~JobSystem();

    JobSystem(const JobSystem&) = delete;
    JobSystem& operator=(const JobSystem&) = delete;

    JobSystem(JobSystem&&) noexcept = delete;
    JobSystem& operator=(JobSystem&&) noexcept = delete;

    // Initialize worker threads. If threadCount == 0, uses hardware_concurrency - 1.
    void initialize(std::uint32_t threadCount = 0);

    // Signal workers to stop and join all threads.
    void shutdown();

    bool is_initialized() const { return m_initialized; }

    // Submit a single fire-and-forget job.
    void submit(Job job);

    // Submit a batch of jobs with a JobHandle that can be waited on.
    JobHandle submit_batch(const std::vector<Job>& jobs);

    // Convenience: parallel for over integer range [begin, end).
    template <typename Func>
    JobHandle parallel_for(std::int32_t begin, std::int32_t end, Func&& func) {
        if (end <= begin) {
            return {};
        }

        const std::int32_t count = end - begin;
        const std::int32_t workerCount = static_cast<std::int32_t>(m_threads.size());
        const std::int32_t chunkCount = workerCount > 0 ? workerCount * 2 : 1;
        const std::int32_t chunkSize = std::max<std::int32_t>(1, count / chunkCount);

        std::vector<Job> jobs;
        jobs.reserve(static_cast<std::size_t>(chunkCount));

        for (std::int32_t start = begin; start < end; start += chunkSize) {
            const std::int32_t chunkEnd = std::min(end, start + chunkSize);
            jobs.emplace_back([start, chunkEnd, fn = func]() {
                for (std::int32_t i = start; i < chunkEnd; ++i) {
                    fn(i);
                }
            });
        }

        return submit_batch(jobs);
    }

private:
    void worker_loop(std::stop_token stopToken);

    Job pop_job();

private:
    std::vector<std::jthread> m_threads;

    std::queue<Job>           m_queue;
    std::mutex                m_queueMutex;
    std::condition_variable_any m_queueCv;

    std::atomic<bool>         m_initialized{false};

    // For batch handles: each batch gets its own shared counter.
    // Workers decrement the counter when finishing a job associated with that batch.
    struct InternalJob {
        Job job;
        std::shared_ptr<std::atomic<std::uint32_t>> counter; // may be null
    };

    std::queue<InternalJob> m_internalQueue;
};

} // namespace wave::engine::core::jobs
