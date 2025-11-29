#include "job_system.hpp"

#include <cassert>
#include <thread>

namespace wave::engine::core::jobs {

// -----------------------------------------------------------------------------
// JobHandle
// -----------------------------------------------------------------------------

void JobHandle::wait() const {
    if (!m_counter) {
        return;
    }

    // Simple spin + sleep hybrid to avoid busy waiting too hard.
    // This can be replaced with a condition_variable-based fence later.
    while (m_counter->load(std::memory_order_acquire) > 0) {
        std::this_thread::yield();
    }
}

// -----------------------------------------------------------------------------
// JobSystem
// -----------------------------------------------------------------------------

JobSystem::~JobSystem() {
    shutdown();
}

void JobSystem::initialize(std::uint32_t threadCount) {
    if (m_initialized.load(std::memory_order_acquire)) {
        return;
    }

    if (threadCount == 0) {
        const std::uint32_t hw = std::thread::hardware_concurrency();
        threadCount = hw > 1 ? hw - 1 : 1;
    }

    m_threads.reserve(threadCount);
    for (std::uint32_t i = 0; i < threadCount; ++i) {
        m_threads.emplace_back(
            [this](std::stop_token stopToken) {
                worker_loop(stopToken);
            }
        );
    }

    m_initialized.store(true, std::memory_order_release);
}

void JobSystem::shutdown() {
    if (!m_initialized.exchange(false, std::memory_order_acq_rel)) {
        return;
    }

    // Request stop on all workers; jthread handles joining automatically.
    for (auto& t : m_threads) {
        t.request_stop();
    }
    m_threads.clear();

    // Clear any remaining jobs.
    {
        std::scoped_lock lock(m_queueMutex);
        while (!m_internalQueue.empty()) {
            m_internalQueue.pop();
        }
    }
}

void JobSystem::submit(Job job) {
    if (!job) {
        return;
    }

    auto internal = InternalJob{};
    internal.job = std::move(job);
    internal.counter.reset();

    {
        std::scoped_lock lock(m_queueMutex);
        m_internalQueue.push(std::move(internal));
    }
    m_queueCv.notify_one();
}

JobHandle JobSystem::submit_batch(const std::vector<Job>& jobs) {
    if (jobs.empty()) {
        return {};
    }

    auto counter = std::make_shared<std::atomic<std::uint32_t>>(
        static_cast<std::uint32_t>(jobs.size())
    );

    {
        std::scoped_lock lock(m_queueMutex);
        for (const auto& j : jobs) {
            if (!j) {
                counter->fetch_sub(1u, std::memory_order_acq_rel);
                continue;
            }

            InternalJob internal{};
            internal.job     = j;
            internal.counter = counter;
            m_internalQueue.push(std::move(internal));
        }
    }

    m_queueCv.notify_all();

    return JobHandle{counter};
}

void JobSystem::worker_loop(std::stop_token stopToken) {
    while (!stopToken.stop_requested()) {
        InternalJob internalJob;

        {
            std::unique_lock lock(m_queueMutex);

            m_queueCv.wait(lock, stopToken, [this]() {
                return !m_internalQueue.empty();
            });

            if (stopToken.stop_requested()) {
                return;
            }

            if (m_internalQueue.empty()) {
                continue;
            }

            internalJob = std::move(m_internalQueue.front());
            m_internalQueue.pop();
        }

        if (internalJob.job) {
            internalJob.job();
        }

        if (internalJob.counter) {
            internalJob.counter->fetch_sub(1u, std::memory_order_acq_rel);
        }
    }
}

} // namespace wave::engine::core::jobs
