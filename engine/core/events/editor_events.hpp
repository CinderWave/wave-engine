#pragma once

#include <cstdint>

#include "engine/core/events/event.hpp"
#include "engine/core/math/vec2.hpp"

namespace wave::engine::core::events
{
    using wave::engine::core::math::Vec2;

    // -------------------------------------------------------------------------
    // Window events
    // -------------------------------------------------------------------------

    struct WindowClosedEvent : public BasicEvent<WindowClosedEvent>
    {
        static constexpr std::string_view Name = "WindowClosedEvent";
    };

    struct WindowResizedEvent : public BasicEvent<WindowResizedEvent>
    {
        static constexpr std::string_view Name = "WindowResizedEvent";

        std::uint32_t width  = 0;
        std::uint32_t height = 0;
    };

    // -------------------------------------------------------------------------
    // Input events (optional complement to InputSystem state)
    // -------------------------------------------------------------------------

    struct MouseMovedEvent : public BasicEvent<MouseMovedEvent>
    {
        static constexpr std::string_view Name = "MouseMovedEvent";

        Vec2 position {0.0f, 0.0f};
        Vec2 delta    {0.0f, 0.0f};
    };

    struct KeyEvent : public BasicEvent<KeyEvent>
    {
        static constexpr std::string_view Name = "KeyEvent";

        // Just a raw code for now; you can map this to your Key enum later
        // or extend this type.
        int key      = 0;
        bool pressed = false;
        bool released = false;
    };

} // namespace wave::engine::core::events
