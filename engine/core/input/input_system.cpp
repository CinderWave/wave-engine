#include "engine/core/input/input_system.hpp"

namespace wave::engine::core::input
{
    InputState InputSystem::s_state{};
    bool       InputSystem::s_initialized = false;

    void InputSystem::initialize() noexcept
    {
        if (s_initialized)
            return;

        s_state = InputState{};
        s_initialized = true;
    }

    void InputSystem::shutdown() noexcept
    {
        if (!s_initialized)
            return;

        s_initialized = false;
    }

    void InputSystem::begin_frame() noexcept
    {
        if (!s_initialized)
            return;

        s_state.begin_frame();
    }

    InputState& InputSystem::state() noexcept
    {
        return s_state;
    }

    void InputSystem::key_press(Key k) noexcept
    {
        if (!s_initialized)
            return;

        s_state.key_press(k);
    }

    void InputSystem::key_release(Key k) noexcept
    {
        if (!s_initialized)
            return;

        s_state.key_release(k);
    }

    void InputSystem::mouse_press(MouseButton b) noexcept
    {
        if (!s_initialized)
            return;

        s_state.mouse_press(b);
    }

    void InputSystem::mouse_release(MouseButton b) noexcept
    {
        if (!s_initialized)
            return;

        s_state.mouse_release(b);
    }

    void InputSystem::mouse_move(float x, float y) noexcept
    {
        if (!s_initialized)
            return;

        s_state.mouse_move(x, y);
    }

    bool InputSystem::is_initialized() noexcept
    {
        return s_initialized;
    }

} // namespace wave::engine::core::input
