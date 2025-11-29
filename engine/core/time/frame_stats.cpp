#include "engine/core/time/frame_stats.hpp"

namespace wave::engine::core::time
{
    void FrameStats::reset() noexcept
    {
        m_accumulatedTime = 0.0f;
        m_frameCount      = 0;
        m_fps             = 0.0f;
    }

    void FrameStats::update(float dt) noexcept
    {
        if (dt <= 0.0f)
            return;

        m_accumulatedTime += dt;
        ++m_frameCount;

        if (m_accumulatedTime >= m_sampleInterval && m_frameCount > 0)
        {
            m_fps = static_cast<float>(m_frameCount) / m_accumulatedTime;
            m_accumulatedTime = 0.0f;
            m_frameCount      = 0;
        }
    }

} // namespace wave::engine::core::time
