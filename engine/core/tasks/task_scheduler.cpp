#include "task_scheduler.hpp"

namespace wave::engine::core::tasks {

// -----------------------------------------------------------------------------
// Task
// -----------------------------------------------------------------------------

void Task::run(TimePoint now) {
    if (!m_func || m_finished) {
        return;
    }

    // Run user code
    m_func();

    // If not recurring: mark finished
    if (!m_recurring) {
        m_finished = true;
        return;
    }

    // Recurring tasks:
    if (m_remaining > 0) {
        m_remaining--;
        if (m_remaining == 0) {
            m_finished = true;
            return;
        }
    }

    // Schedule next execution
    m_nextRun = now + m_interval;
}

// -----------------------------------------------------------------------------
// TaskScheduler
// -----------------------------------------------------------------------------

void TaskScheduler::update() {
    const TimePoint now = Clock::now();

    std::scoped_lock lock(m_mutex);

    for (Task& t : m_tasks) {
        if (!t.valid()) {
            continue;
        }

        if (t.due(now)) {
            t.run(now);
        }
    }

    // Remove finished tasks
    m_tasks.erase(
        std::remove_if(
            m_tasks.begin(),
            m_tasks.end(),
            [](const Task& t) { return t.finished(); }
        ),
        m_tasks.end()
    );
}

void TaskScheduler::schedule_next_frame(TaskFunc func) {
    if (!func) {
        return;
    }

    const TimePoint now = Clock::now();
    Task t(
        std::move(func),
        false,                 // non-recurring
        0,
        Duration::zero(),
        now                    // run next update
    );

    std::scoped_lock lock(m_mutex);
    m_tasks.emplace_back(std::move(t));
}

void TaskScheduler::schedule_delay(TaskFunc func, std::uint64_t delayMs) {
    if (!func) {
        return;
    }

    const TimePoint now = Clock::now();
    Duration delay = std::chrono::milliseconds(delayMs);

    Task t(
        std::move(func),
        false,         // one-shot
        0,
        delay,
        now + delay
    );

    std::scoped_lock lock(m_mutex);
    m_tasks.emplace_back(std::move(t));
}

void TaskScheduler::schedule_interval(TaskFunc func,
                                      std::uint64_t intervalMs,
                                      std::uint32_t repeatCount) {
    if (!func) {
        return;
    }

    const TimePoint now = Clock::now();
    Duration interval = std::chrono::milliseconds(intervalMs);

    Task t(
        std::move(func),
        true,               // recurring
        repeatCount,
        interval,
        now + interval
    );

    std::scoped_lock lock(m_mutex);
    m_tasks.emplace_back(std::move(t));
}

} // namespace wave::engine::core::tasks
