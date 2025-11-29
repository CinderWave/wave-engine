#include "engine/platform/glfw/glfw_window.hpp"

#include <stdexcept>
#include <iostream>

#include "engine/platform/glfw/glfw_input_adapter.hpp"
#include "engine/core/events/event_system.hpp"
#include "engine/core/events/editor_events.hpp"

namespace wave::engine::platform::glfw
{
    using wave::engine::core::events::EventSystem;
    using wave::engine::core::events::WindowClosedEvent;
    using wave::engine::core::events::WindowResizedEvent;

    int GlfwWindow::s_glfw_ref_count = 0;

    static void glfw_error_callback(int error, const char* description)
    {
        std::cerr << "[GLFW] Error " << error << ": " << description << "\n";
    }

    void GlfwWindow::ensure_glfw_initialized()
    {
        if (s_glfw_ref_count == 0)
        {
            glfwSetErrorCallback(glfw_error_callback);

            if (!glfwInit())
                throw std::runtime_error("Failed to initialize GLFW");
        }

        ++s_glfw_ref_count;
    }

    void GlfwWindow::shutdown_glfw_if_needed()
    {
        --s_glfw_ref_count;
        if (s_glfw_ref_count <= 0)
        {
            glfwTerminate();
            s_glfw_ref_count = 0;
        }
    }

    GlfwWindow::GlfwWindow(std::uint32_t width,
                           std::uint32_t height,
                           const std::string& title)
        : m_window(nullptr)
        , m_width(width)
        , m_height(height)
        , m_title(title)
    {
        ensure_glfw_initialized();

        // Basic window hints.
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Vulkan, not OpenGL.
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_window = glfwCreateWindow(
            static_cast<int>(m_width),
            static_cast<int>(m_height),
            m_title.c_str(),
            nullptr,
            nullptr
        );

        if (!m_window)
        {
            shutdown_glfw_if_needed();
            throw std::runtime_error("Failed to create GLFW window");
        }

        // Store 'this' so callbacks can access instance data (size, etc.).
        glfwSetWindowUserPointer(m_window, this);

        // Bind input callbacks for this window.
        GlfwInputAdapter::bind_callbacks(m_window);

        // Bind window-level callbacks for events.
        glfwSetWindowCloseCallback(m_window, window_close_callback);
        glfwSetWindowSizeCallback(m_window, window_size_callback);
    }

    GlfwWindow::~GlfwWindow()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }

        shutdown_glfw_if_needed();
    }

    void GlfwWindow::poll_events() const noexcept
    {
        glfwPollEvents();
    }

    bool GlfwWindow::should_close() const noexcept
    {
        if (!m_window)
            return true;

        return glfwWindowShouldClose(m_window) == GLFW_TRUE;
    }

    void GlfwWindow::window_close_callback(GLFWwindow* window)
    {
        // Notify the engine/editor that this window is closing.
        if (EventSystem::is_initialized())
        {
            WindowClosedEvent e;
            EventSystem::publish(e);
        }

        // Mark window as should-close (GLFW usually does this automatically).
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    void GlfwWindow::window_size_callback(GLFWwindow* window, int width, int height)
    {
        // Update the GlfwWindow instance dimensions.
        if (auto* self = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window)))
        {
            self->m_width  = static_cast<std::uint32_t>(width);
            self->m_height = static_cast<std::uint32_t>(height);
        }

        // Broadcast a resize event to the engine/editor.
        if (EventSystem::is_initialized())
        {
            WindowResizedEvent e;
            e.width  = static_cast<std::uint32_t>(width);
            e.height = static_cast<std::uint32_t>(height);
            EventSystem::publish(e);
        }
    }

    void GlfwWindow::set_title(const std::string& title) noexcept
    {
        m_title = title;

        if (m_window)
        {
            glfwSetWindowTitle(m_window, m_title.c_str());
        }
    }

} // namespace wave::engine::platform::glfw
