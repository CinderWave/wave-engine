#include "engine/core/input/input_state.hpp"

namespace wave::engine::core::input
{
    void InputState::begin_frame() noexcept
    {
        pressed.fill(false);
        released.fill(false);

        mouse_pressed.fill(false);
        mouse_released.fill(false);

        mouse_delta_x = 0.0f;
        mouse_delta_y = 0.0f;
    }

    void InputState::key_press(Key k) noexcept
    {
        auto idx = static_cast<size_t>(k);
        if (idx >= down.size())
            return;

        if (!down[idx])
            pressed[idx] = true;

        down[idx] = true;
    }

    void InputState::key_release(Key k) noexcept
    {
        auto idx = static_cast<size_t>(k);
        if (idx >= down.size())
            return;

        if (down[idx])
            released[idx] = true;

        down[idx] = false;
    }

    void InputState::mouse_press(MouseButton b) noexcept
    {
        auto idx = static_cast<size_t>(b);
        if (idx >= mouse_down.size())
            return;

        if (!mouse_down[idx])
            mouse_pressed[idx] = true;

        mouse_down[idx] = true;
    }

    void InputState::mouse_release(MouseButton b) noexcept
    {
        auto idx = static_cast<size_t>(b);
        if (idx >= mouse_down.size())
            return;

        if (mouse_down[idx])
            mouse_released[idx] = true;

        mouse_down[idx] = false;
    }

    void InputState::mouse_move(float x, float y) noexcept
    {
        mouse_delta_x = x - mouse_x;
        mouse_delta_y = y - mouse_y;

        mouse_x = x;
        mouse_y = y;
    }

} // namespace wave::engine::core::input
