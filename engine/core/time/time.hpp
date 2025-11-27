#pragma once

#include <cstdint>

namespace wave::engine::core::time
{
    // Simple high-res timing module.
    // Provides:
    //   - Real time (seconds since engine start)
    //   - Delta time
    //   - Millisecond timestamp
    //   - Reset capability (useful for profiling)

    class Time
    {
    public:
        static void initialize() noexcept;
        static void update() noexcept;

        // Seconds since initialize() was called.
        static double time_since_start() noexcept;

        // Seconds between update() calls.
        static double delta_seconds() noexcept;

        // Milliseconds since initialize().
        static std::uint64_t milliseconds_since_start() noexcept;

    private:
        static double s_start_time;
        static double s_last_frame_time;
        static double s_delta;
    };

} // namespace wave::engine::core::time
