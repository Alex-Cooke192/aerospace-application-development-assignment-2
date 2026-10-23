#include <functional>
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <any>

#pragma once

class EventBus
{
public:
    template<typename Event>
    void subscribe(std::function<void(const Event&)> handler)
    {
        auto wrapper = [handler](const std::any& event)
        {
            handler(std::any_cast<const Event&>(event));
        };

        subscribers[typeid(Event)].push_back(wrapper);
    }

    template<typename Event>
    void publish(const Event& event)
    {
        auto it = subscribers.find(typeid(Event));

        if (it == subscribers.end())
        {
            return;
        }

        for (auto& handler : it->second)
        {
            handler(event);
        }
    }

private:
    using Handler = std::function<void(const std::any&)>;

    std::unordered_map<
        std::type_index,
        std::vector<Handler>
    > subscribers;
};