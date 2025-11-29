#pragma once

#include "engine/core/events/event_bus.hpp"

namespace wave::engine::core::events
{
    // Global event system facade around a single EventBus instance.
    //
    // Usage:
    //   EventSystem::initialize();
    //   EventSystem::subscribe<WindowClosedEvent>(...);
    //   EventSystem::publish(WindowClosedEvent{ ... });
    //
    // Shutdown is optional but recommended on engine teardown.
    class EventSystem
    {
    public:
        EventSystem() = delete;
        ~EventSystem() = delete;
        EventSystem(const EventSystem&) = delete;
        EventSystem(EventSystem&&) = delete;
        EventSystem& operator=(const EventSystem&) = delete;
        EventSystem& operator=(EventSystem&&) = delete;

        static void initialize() noexcept;
        static void shutdown() noexcept;

        static bool is_initialized() noexcept;

        // Access the global event bus.
        static EventBus& bus() noexcept;

        // Convenience forwards.

        template <typename T>
        static void subscribe(typename EventBus::Listener listener)
        {
            if (!s_initialized)
                return;

            s_bus.subscribe<T>(std::move(listener));
        }

        template <typename T>
        static void publish(const T& event)
        {
            if (!s_initialized)
                return;

            s_bus.publish<T>(event);
        }

    private:
        static EventBus s_bus;
        static bool     s_initialized;
    };

} // namespace wave::engine::core::events
