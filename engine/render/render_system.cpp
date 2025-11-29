#include "engine/render/render_system.hpp"

#include "engine/core/logging/log.hpp"
#include "engine/render/vulkan/vk_backend.hpp"

namespace wave::engine::render
{
    using wave::engine::core::logging::Logger;
    using wave::engine::render::vulkan::VkBackend;

    std::unique_ptr<RenderBackend> RenderSystem::s_backend{};
    bool                           RenderSystem::s_initialized = false;

    bool RenderSystem::initialize(void* window_handle,
                                  std::uint32_t width,
                                  std::uint32_t height)
    {
        if (s_initialized)
            return true;

        if (!window_handle)
        {
            WAVE_LOG_ERROR("[render] RenderSystem::initialize called with null window handle.");
            return false;
        }

        auto backend = std::make_unique<VkBackend>();

        RenderInitInfo info{};
        info.window_handle = window_handle;
        info.width         = width;
        info.height        = height;

        if (!backend->initialize(info))
        {
            WAVE_LOG_ERROR("[render] Failed to initialize render backend.");
            return false;
        }

        s_backend     = std::move(backend);
        s_initialized = true;

        WAVE_LOG_INFO("[render] RenderSystem initialized.");
        return true;
    }

    void RenderSystem::shutdown() noexcept
    {
        if (!s_initialized)
            return;

        if (s_backend)
        {
            s_backend->shutdown();
            s_backend.reset();
        }

        s_initialized = false;
        WAVE_LOG_INFO("[render] RenderSystem shutdown complete.");
    }

    bool RenderSystem::is_initialized() noexcept
    {
        return s_initialized;
    }

    void RenderSystem::begin_frame()
    {
        if (!s_initialized || !s_backend)
            return;

        s_backend->begin_frame();
    }

    void RenderSystem::end_frame()
    {
        if (!s_initialized || !s_backend)
            return;

        s_backend->end_frame();
    }

    void RenderSystem::resize(std::uint32_t width, std::uint32_t height)
    {
        if (!s_initialized || !s_backend)
            return;

        s_backend->resize(width, height);
    }

    RenderBackend* RenderSystem::backend() noexcept
    {
        return s_backend.get();
    }

} // namespace wave::engine::render
