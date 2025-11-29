#pragma once

#include <functional>
#include <vector>
#include <chrono>
#include <cstdint>
#include <mutex>

namespace wave::engine::core::tasks {

using Clock     = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using Duration  = Clock::duration;

using TaskFunc = std::function<void()>;

// Defines a scheduled task.
// A task can run:
//   - Every frame
//   - After a delay
//   - At a fixed time interval
//   - Repeated N times or infinite
class Task {
public:
    Task() = default;

    Task(TaskFunc func,
         bool recurring,
        std::uint32_t repeatCount,
         Duration interval,
         TimePoint nextRun)
        : m_func(std::move(func))
        , m_recurring(recurring)
        , m_remaining(repeatCount)
        , m_interval(interval)
        , m_nextRun(nextRun) {}

    bool valid() const { return static_cast<bool>(m_func); }

    // Returns true if the task should run at this time.
    bool due(TimePoint now) const {
        return now >= m_nextRun;
    }

    // Run the task and update nextRun / remaining count.
    void run(TimePoint now);

    bool finished() const { return m_finished; }

private:
    TaskFunc m_func;
    bool     m_recurring{false};
    std::uint32_t m_remaining{0}; // 0 means infinite
    Duration m_interval{Duration::zero()};
    TimePoint m_nextRun{};
    bool m_finished{false};
};

// Simple scheduler for engine-level periodic work.
// Owned by the runtime or engine core.
class TaskScheduler final {
public:
    TaskScheduler() = default;
    ~TaskScheduler() = default;

    TaskScheduler(const TaskScheduler&) = delete;
    TaskScheduler& operator=(const TaskScheduler&) = delete;

    TaskScheduler(TaskScheduler&&) noexcept = delete;
    TaskScheduler& operator=(TaskScheduler&&) noexcept = delete;

    // Called every frame by the runtime.
    void update();

    // One-shot task that runs next frame.
    void schedule_next_frame(TaskFunc func);

    // One-shot delay task: runs after delayMs.
    void schedule_delay(TaskFunc func, std::uint64_t delayMs);

    // Repeating task: runs every intervalMs, optionally a fixed number of times.
    void schedule_interval(TaskFunc func,
                           std::uint64_t intervalMs,
                           std::uint32_t repeatCount = 0); // 0 = infinite

private:
    std::vector<Task> m_tasks;
    std::mutex        m_mutex;
};

} // namespace wave::engine::core::tasks
