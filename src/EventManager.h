#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

enum class EventType {
    kKeyDown = sf::Event::KeyPressed,
    kKeyUp = sf::Event::KeyReleased,
    kMouseButtonDown = sf::Event::MouseButtonPressed,
    kMouseButtonUp = sf::Event::MouseButtonReleased,
    kMouseWheel = sf::Event::MouseWheelMoved,
    kWindowResized = sf::Event::Resized,
    kGainedFocus = sf::Event::GainedFocus,
    kLostFocus = sf::Event::LostFocus,
    kMouseEntered = sf::Event::MouseEntered,
    kMouseLeft = sf::Event::MouseLeft,
    kClosed = sf::Event::Closed,
    kTextEntered = sf::Event::TextEntered,
    kMouseMoved = sf::Event::MouseMoved,
    kKeyboard = sf::Event::Count + 1,
    kMouse,
    kJoystick
};

struct BoundEventInfo {
    BoundEventInfo() = default;
    explicit BoundEventInfo(const int event)
        : key_code(event){};

    union {
        int key_code = -1;
    };
};

struct EventDetails {
    explicit EventDetails(const std::string bind_name)
        : name_(std::move(bind_name))
    {
        Clear();
    }

    std::string name_;

    sf::Vector2i size_;
    sf::Uint32 text_entered_;
    sf::Vector2i mouse_position_;
    int mouse_wheel_delta_;
    int key_code_;

    void Clear()
    {
        size_ = sf::Vector2i(0, 0);
        text_entered_ = 0;
        mouse_position_ = sf::Vector2i(0, 0);
        mouse_wheel_delta_ = 0;
        key_code_ = -1;
    }
};

using Events = std::vector<std::pair<EventType, BoundEventInfo>>;

struct Binding {
    explicit Binding(const std::string& name)
        : name_(name)
        , details_(name)
    {
    }

    void BindEvent(EventType type, BoundEventInfo info = BoundEventInfo())
    {
        events_.emplace_back(type, info);
    }

    Events events_;
    std::string name_;
    int event_matches_ = 0;

    EventDetails details_;
};

enum class StateType;

class EventManager {
public:
    using BindingsMap = std::unordered_map<std::string, std::unique_ptr<Binding>>;
    using CallbackContainer = std::unordered_map<std::string, std::function<void(EventDetails*)>>;
    using Callbacks = std::unordered_map<StateType, CallbackContainer>;

    EventManager();
    ~EventManager();

    void AddBinding(std::unique_ptr<Binding> binding);
    void RemoveBinding(const std::string name);

    void SetFocus(const bool& focus);

    template <class T>
    bool AddCallback(StateType state, const std::string& name,
        void (T::*function)(const EventDetails* const),
        T* instance)
    {
        auto itr = callbacks_.emplace(state, CallbackContainer()).first;

        auto temp = std::bind(function, instance, std::placeholders::_1);

        return itr->second.emplace(name, temp).second;
    }

    bool RemoveCallback(StateType state, const std::string& name)
    {
        auto itr = callbacks_.find(state);
        if (itr == callbacks_.end()) {
            return false;
        }
        if (itr->second.erase(name) == 0) {
            return false;
        }
        return true;
    }

    void SetCurrentState(const StateType& state);
    void ProcessDiscreteEvent(sf::Event& event);
    void ProcessContinuousEventsAndUpdate();

    sf::Vector2f GetMousePosition(sf::RenderWindow* window = nullptr) const
    {
        return (window ? sf::Vector2f(sf::Mouse::getPosition(*window))
                       : sf::Vector2f(sf::Mouse::getPosition()));
    }

private:
    void LoadBindings();

    StateType current_state_;
    BindingsMap bindings_;
    Callbacks callbacks_;
    bool has_focus_ = true;
};
#endif // EVENTMANAGER_H
