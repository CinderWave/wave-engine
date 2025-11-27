#pragma once

#include <GLFW/glfw3.h>

#include "engine/core/input/input_system.hpp"

namespace wave::engine::platform::glfw
{
    // Platform layer that translates GLFW events into Wave Engine input events.
    //
    // Call bind_callbacks() right after creating your GLFW window.
    class GlfwInputAdapter
    {
    public:
        GlfwInputAdapter() = delete;

        // Attach all GLFW callbacks to the given window.
        static void bind_callbacks(GLFWwindow* window);

    private:
        // Individual callback hooks
        static void key_callback(GLFWwindow* window,
                                 int key,
                                 int scancode,
                                 int action,
                                 int mods);

        static void mouse_button_callback(GLFWwindow* window,
                                          int button,
                                          int action,
                                          int mods);

        static void cursor_pos_callback(GLFWwindow* window,
                                        double xpos,
                                        double ypos);
    };

} // namespace wave::engine::platform::glfw
