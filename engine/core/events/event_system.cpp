#include "engine/core/events/event_system.hpp"

namespace wave::engine::core::events
{
    EventBus EventSystem::s_bus{};
    bool     EventSystem::s_initialized = false;

    void EventSystem::initialize() noexcept
    {
        if (s_initialized)
            return;

        s_bus         = EventBus{};
        s_initialized = true;
    }

    void EventSystem::shutdown() noexcept
    {
        if (!s_initialized)
            return;

        // Currently nothing special to tear down.
        s_initialized = false;
    }

    bool EventSystem::is_initialized() noexcept
    {
        return s_initialized;
    }

    EventBus& EventSystem::bus() noexcept
    {
        return s_bus;
    }

} // namespace wave::engine::core::events
