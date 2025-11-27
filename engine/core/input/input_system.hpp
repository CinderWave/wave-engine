#pragma once

#include "engine/core/input/input_state.hpp"

namespace wave::engine::core::input
{
    // Central input system managing a single global InputState instance.
    //
    // Platform and windowing code will feed events into this system.
    // Gameplay, editor and tools will query it.
    class InputSystem
    {
    public:
        InputSystem() = delete;
        ~InputSystem() = delete;
        InputSystem(const InputSystem&) = delete;
        InputSystem(InputSystem&&) = delete;
        InputSystem& operator=(const InputSystem&) = delete;
        InputSystem& operator=(InputSystem&&) = delete;

        // Initialize the global input state.
        // Safe to call more than once; subsequent calls are no-ops.
        static void initialize() noexcept;

        // Shutdown the input system.
        // Safe to call even if initialize() was never called.
        static void shutdown() noexcept;

        // Called once per frame at the start of the frame.
        // Clears one-frame events such as pressed / released and mouse delta.
        static void begin_frame() noexcept;

        // Access the mutable global input state.
        // Valid after initialize().
        static InputState& state() noexcept;

        // Convenience forwarding helpers for event injection.
        static void key_press(Key k) noexcept;
        static void key_release(Key k) noexcept;

        static void mouse_press(MouseButton b) noexcept;
        static void mouse_release(MouseButton b) noexcept;
        static void mouse_move(float x, float y) noexcept;

        // Has the system been initialized.
        static bool is_initialized() noexcept;

    private:
        static InputState s_state;
        static bool       s_initialized;
    };

} // namespace wave::engine::core::input
