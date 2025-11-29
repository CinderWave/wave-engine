#pragma once

#include <cstdint>

namespace wave::engine::render
{
    struct RenderInitInfo
    {
        void* window_handle = nullptr; // platform-specific pointer (GLFWwindow*)
        std::uint32_t width  = 0;
        std::uint32_t height = 0;
    };

    class RenderBackend
    {
    public:
        virtual ~RenderBackend() = default;

        // Initialize the backend using window info.
        virtual bool initialize(const RenderInitInfo& info) = 0;

        // Called every frame.
        virtual void begin_frame() = 0;
        virtual void end_frame()   = 0;

        // Resize callback
        virtual void resize(std::uint32_t width, std::uint32_t height) = 0;

        // Shutdown
        virtual void shutdown() = 0;
    };

} // namespace wave::engine::render
