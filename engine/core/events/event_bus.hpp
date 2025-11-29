#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>

#include "event.hpp"

namespace wave::engine::core::events
{
    class EventBus
    {
    public:
        using Listener = std::function<void(const Event&)>;

        // Register a listener for a specific event type.
        template<typename T>
        void subscribe(Listener listener)
        {
            auto key = std::type_index(typeid(T));
            listeners[key].push_back(std::move(listener));
        }

        // Publish an event instance. Bus delivers it to all listeners.
        template<typename T>
        void publish(const T& event)
        {
            auto key = std::type_index(typeid(T));
            auto it = listeners.find(key);
            if (it == listeners.end())
                return;

            for (auto& fn : it->second)
                fn(event);
        }

    private:
        std::unordered_map<std::type_index, std::vector<Listener>> listeners;
    };

} // namespace wave::engine::core::events
