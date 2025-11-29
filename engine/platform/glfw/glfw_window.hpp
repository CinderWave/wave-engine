#pragma once

#include <string>
#include <cstdint>

#include <GLFW/glfw3.h>

namespace wave::engine::platform::glfw
{
    // Simple RAII wrapper around a GLFW window.
    //
    // Responsibilities:
    //  - Initialize GLFW (once) when the first window is created.
    //  - Create and destroy the GLFWwindow handle.
    //  - Provide basic event polling and close checks.
    //  - Emit core window events via the EventSystem (close, resize).
    class GlfwWindow
    {
    public:
        GlfwWindow(std::uint32_t width,
                   std::uint32_t height,
                   const std::string& title);

        ~GlfwWindow();

        GlfwWindow(const GlfwWindow&)            = delete;
        GlfwWindow& operator=(const GlfwWindow&) = delete;
        GlfwWindow(GlfwWindow&&)                 = delete;
        GlfwWindow& operator=(GlfwWindow&&)      = delete;

        // Poll OS events. Call once per frame.
        void poll_events() const noexcept;

        // Has the user requested the window to close.
        bool should_close() const noexcept;

        // Access the underlying GLFW handle.
        GLFWwindow* handle() const noexcept { return m_window; }

        // Get window size.
        std::uint32_t width() const noexcept  { return m_width; }
        std::uint32_t height() const noexcept { return m_height; }

        // Change window title at runtime.
        void set_title(const std::string& title) noexcept;

    private:
        static void ensure_glfw_initialized();
        static void shutdown_glfw_if_needed();

        static int s_glfw_ref_count;

        // GLFW callbacks
        static void window_close_callback(GLFWwindow* window);
        static void window_size_callback(GLFWwindow* window, int width, int height);

        GLFWwindow*   m_window   = nullptr;
        std::uint32_t m_width    = 0;
        std::uint32_t m_height   = 0;
        std::string   m_title;
    };

} // namespace wave::engine::platform::glfw
