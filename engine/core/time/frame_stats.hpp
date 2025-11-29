#pragma once

#include <cstdint>

namespace wave::engine::core::time
{
    // Simple FPS tracker.
    //
    // Call update(delta_seconds) once per frame.
    // fps() returns a smoothed frames per second value.
    class FrameStats
    {
    public:
        FrameStats() = default;

        void reset() noexcept;

        // dt is frame delta time in seconds.
        void update(float dt) noexcept;

        // Latest calculated frames per second.
        float fps() const noexcept { return m_fps; }

    private:
        float       m_accumulatedTime = 0.0f;
        std::uint32_t m_frameCount    = 0;
        float       m_fps             = 0.0f;
        float       m_sampleInterval  = 0.5f; // seconds for averaging
    };

} // namespace wave::engine::core::time
