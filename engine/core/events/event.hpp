#pragma once

#include <string_view>
#include <cstdint>

namespace wave::engine::core::events
{
    // Base event type. Everything inherits from this.
    struct Event
    {
        virtual ~Event() = default;

        // Optional: readable name for debugging
        virtual std::string_view name() const = 0;
    };

    // Simple typed event for common cases
    template<typename T>
    struct BasicEvent : public Event
    {
        static std::string_view static_name()
        {
            return T::Name;
        }

        std::string_view name() const override
        {
            return T::Name;
        }
    };

} // namespace wave::engine::core::events
