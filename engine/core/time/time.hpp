#pragma once

#include <cstdint>

namespace wave::engine::core::time
{
    // Time module
    //
    // Usage:
    //  - Call Time::initialize() ONCE during engine startup.
    //  - Call Time::update() once per frame.
    //  - Query delta_seconds() and total_seconds() anywhere.
    //
    class Time
    {
    public:
        Time() = delete;

        static void initialize() noexcept;
        static void update() noexcept;

        // Time since last frame in seconds.
        static float delta_seconds() noexcept { return s_deltaSeconds; }

        // Total time since engine start in seconds.
        static float total_seconds() noexcept { return s_totalSeconds; }

        // Engine-wide frame counter.
        static std::uint64_t frame_count() noexcept { return s_frameCount; }

    private:
        static double        s_lastTimestamp;
        static float         s_deltaSeconds;
        static float         s_totalSeconds;
        static std::uint64_t s_frameCount;
    };

} // namespace wave::engine::core::time
