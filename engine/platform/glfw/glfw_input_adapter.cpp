#include "engine/platform/glfw/glfw_input_adapter.hpp"

namespace wave::engine::platform::glfw
{
    using wave::engine::core::input::InputSystem;
    using wave::engine::core::input::Key;
    using wave::engine::core::input::MouseButton;

    // Map GLFW key codes to Wave Engine Key enum.
    static Key translate_key(int glfw_key)
    {
        switch (glfw_key)
        {
            case GLFW_KEY_A: return Key::A;
            case GLFW_KEY_B: return Key::B;
            case GLFW_KEY_C: return Key::C;
            case GLFW_KEY_D: return Key::D;
            case GLFW_KEY_E: return Key::E;
            case GLFW_KEY_F: return Key::F;
            case GLFW_KEY_G: return Key::G;
            case GLFW_KEY_H: return Key::H;
            case GLFW_KEY_I: return Key::I;
            case GLFW_KEY_J: return Key::J;
            case GLFW_KEY_K: return Key::K;
            case GLFW_KEY_L: return Key::L;
            case GLFW_KEY_M: return Key::M;
            case GLFW_KEY_N: return Key::N;
            case GLFW_KEY_O: return Key::O;
            case GLFW_KEY_P: return Key::P;
            case GLFW_KEY_Q: return Key::Q;
            case GLFW_KEY_R: return Key::R;
            case GLFW_KEY_S: return Key::S;
            case GLFW_KEY_T: return Key::T;
            case GLFW_KEY_U: return Key::U;
            case GLFW_KEY_V: return Key::V;
            case GLFW_KEY_W: return Key::W;
            case GLFW_KEY_X: return Key::X;
            case GLFW_KEY_Y: return Key::Y;
            case GLFW_KEY_Z: return Key::Z;

            case GLFW_KEY_0: return Key::Num0;
            case GLFW_KEY_1: return Key::Num1;
            case GLFW_KEY_2: return Key::Num2;
            case GLFW_KEY_3: return Key::Num3;
            case GLFW_KEY_4: return Key::Num4;
            case GLFW_KEY_5: return Key::Num5;
            case GLFW_KEY_6: return Key::Num6;
            case GLFW_KEY_7: return Key::Num7;
            case GLFW_KEY_8: return Key::Num8;
            case GLFW_KEY_9: return Key::Num9;

            case GLFW_KEY_ESCAPE: return Key::Escape;
            case GLFW_KEY_SPACE:  return Key::Space;
            case GLFW_KEY_ENTER:  return Key::Enter;
            case GLFW_KEY_LEFT_SHIFT:
            case GLFW_KEY_RIGHT_SHIFT:
                return Key::Shift;

            case GLFW_KEY_LEFT_CONTROL:
            case GLFW_KEY_RIGHT_CONTROL:
                return Key::Control;

            case GLFW_KEY_LEFT_ALT:
            case GLFW_KEY_RIGHT_ALT:
                return Key::Alt;

            case GLFW_KEY_LEFT:  return Key::Left;
            case GLFW_KEY_RIGHT: return Key::Right;
            case GLFW_KEY_UP:    return Key::Up;
            case GLFW_KEY_DOWN:  return Key::Down;
        }

        return Key::Unknown;
    }

    static MouseButton translate_mouse(int glfw_button)
    {
        switch (glfw_button)
        {
            case GLFW_MOUSE_BUTTON_LEFT:   return MouseButton::Left;
            case GLFW_MOUSE_BUTTON_RIGHT:  return MouseButton::Right;
            case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
        }
        return MouseButton::Left;
    }

    void GlfwInputAdapter::bind_callbacks(GLFWwindow* window)
    {
        glfwSetKeyCallback(window, key_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
    }

    void GlfwInputAdapter::key_callback(GLFWwindow*,
                                       int key,
                                       int,
                                       int action,
                                       int)
    {
        Key k = translate_key(key);
        if (k == Key::Unknown)
            return;

        if (action == GLFW_PRESS)
            InputSystem::key_press(k);
        else if (action == GLFW_RELEASE)
            InputSystem::key_release(k);
    }

    void GlfwInputAdapter::mouse_button_callback(GLFWwindow*,
                                                 int button,
                                                 int action,
                                                 int)
    {
        MouseButton b = translate_mouse(button);

        if (action == GLFW_PRESS)
            InputSystem::mouse_press(b);
        else if (action == GLFW_RELEASE)
            InputSystem::mouse_release(b);
    }

    void GlfwInputAdapter::cursor_pos_callback(GLFWwindow*,
                                               double xpos,
                                               double ypos)
    {
        InputSystem::mouse_move(static_cast<float>(xpos),
                                static_cast<float>(ypos));
    }

} // namespace wave::engine::platform::glfw
