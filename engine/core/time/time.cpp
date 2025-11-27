#include "engine/core/time/time.hpp"

#include <chrono>

namespace wave::engine::core::time
{
    double Time::s_start_time       = 0.0;
    double Time::s_last_frame_time  = 0.0;
    double Time::s_delta            = 0.0;

    static double now_seconds() noexcept
    {
        using clock = std::chrono::high_resolution_clock;
        auto tp = clock::now().time_since_epoch();
        return std::chrono::duration<double>(tp).count();
    }

    void Time::initialize() noexcept
    {
        s_start_time      = now_seconds();
        s_last_frame_time = s_start_time;
        s_delta           = 0.0;
    }

    void Time::update() noexcept
    {
        double current = now_seconds();
        s_delta        = current - s_last_frame_time;
        s_last_frame_time = current;
    }

    double Time::time_since_start() noexcept
    {
        return now_seconds() - s_start_time;
    }

    double Time::delta_seconds() noexcept
    {
        return s_delta;
    }

    std::uint64_t Time::milliseconds_since_start() noexcept
    {
        double ms = (now_seconds() - s_start_time) * 1000.0;
        return static_cast<std::uint64_t>(ms);
    }

} // namespace wave::engine::core::time
