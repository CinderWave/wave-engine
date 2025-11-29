#include "engine/core/time/time.hpp"

#include <chrono>

namespace wave::engine::core::time
{
    double        Time::s_lastTimestamp = 0.0;
    float         Time::s_deltaSeconds  = 0.0f;
    float         Time::s_totalSeconds  = 0.0f;
    std::uint64_t Time::s_frameCount    = 0;

    static double now_seconds()
    {
        using namespace std::chrono;
        auto now = high_resolution_clock::now();
        auto sec = time_point_cast<duration<double>>(now);
        return sec.time_since_epoch().count();
    }

    void Time::initialize() noexcept
    {
        s_lastTimestamp = now_seconds();
        s_deltaSeconds  = 0.0f;
        s_totalSeconds  = 0.0f;
        s_frameCount    = 0;
    }

    void Time::update() noexcept
    {
        double current = now_seconds();
        double dt      = current - s_lastTimestamp;

        // Clamp very large delta jumps, helps during window moves or halts.
        if (dt > 0.25)
            dt = 0.25;

        s_deltaSeconds = static_cast<float>(dt);
        s_totalSeconds += s_deltaSeconds;
        s_lastTimestamp = current;
        ++s_frameCount;
    }

} // namespace wave::engine::core::time
