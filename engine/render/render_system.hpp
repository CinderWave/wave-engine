#pragma once

#include <cstdint>
#include <memory>

#include "engine/render/render_backend.hpp"

namespace wave::engine::render
{
    class RenderSystem
    {
    public:
        RenderSystem() = delete;
        ~RenderSystem() = delete;

        RenderSystem(const RenderSystem&) = delete;
        RenderSystem(RenderSystem&&) = delete;
        RenderSystem& operator=(const RenderSystem&) = delete;
        RenderSystem& operator=(RenderSystem&&) = delete;

        // Initialize the render system with a platform window and size.
        static bool initialize(void* window_handle,
                               std::uint32_t width,
                               std::uint32_t height);

        static void shutdown() noexcept;

        static bool is_initialized() noexcept;

        static void begin_frame();
        static void end_frame();
        static void resize(std::uint32_t width, std::uint32_t height);

        // Access underlying backend if needed (internal use).
        static RenderBackend* backend() noexcept;

    private:
        static std::unique_ptr<RenderBackend> s_backend;
        static bool                           s_initialized;
    };

} // namespace wave::engine::render
