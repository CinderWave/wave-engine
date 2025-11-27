#pragma once

#include <array>
#include <cstdint>

namespace wave::engine::core::input
{
    // Basic key enum. Expand or replace later when platform layer plugs in.
    enum class Key : uint16_t
    {
        Unknown = 0,

        // Letters
        A, B, C, D, E, F, G,
        H, I, J, K, L, M, N,
        O, P, Q, R, S, T, U,
        V, W, X, Y, Z,

        // Digits
        Num0, Num1, Num2, Num3, Num4,
        Num5, Num6, Num7, Num8, Num9,

        // Special
        Escape,
        Space,
        Enter,
        Shift,
        Control,
        Alt,

        Left,
        Right,
        Up,
        Down,

        Count // Always last
    };

    enum class MouseButton : uint8_t
    {
        Left = 0,
        Right = 1,
        Middle = 2,
        Count
    };

    struct InputState
    {
        // Keyboard -----------------------------------------------------------
        std::array<bool, (size_t)Key::Count> down{};
        std::array<bool, (size_t)Key::Count> pressed{};
        std::array<bool, (size_t)Key::Count> released{};

        // Mouse --------------------------------------------------------------
        std::array<bool, (size_t)MouseButton::Count> mouse_down{};
        std::array<bool, (size_t)MouseButton::Count> mouse_pressed{};
        std::array<bool, (size_t)MouseButton::Count> mouse_released{};

        float mouse_x = 0.0f;
        float mouse_y = 0.0f;

        float mouse_delta_x = 0.0f;
        float mouse_delta_y = 0.0f;

        // Called at the start of each frame to clean one-frame events
        void begin_frame() noexcept;

        // Keyboard event injection
        void key_press(Key k) noexcept;
        void key_release(Key k) noexcept;

        // Mouse event injection
        void mouse_press(MouseButton b) noexcept;
        void mouse_release(MouseButton b) noexcept;
        void mouse_move(float x, float y) noexcept;
    };

} // namespace wave::engine::core::input
