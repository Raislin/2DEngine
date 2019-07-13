#include "EventManager.h"
#include "Utilities.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

EventManager::EventManager()
    : current_state_(StateType(0))
{
    LoadBindings();
}

EventManager::~EventManager() {}

void EventManager::AddBinding(std::unique_ptr<Binding> binding)
{
    if (bindings_.find(binding->name_) == bindings_.end()) {
        bindings_.emplace(binding->name_, std::move(binding));
    }
}

void EventManager::RemoveBinding(const std::string name)
{
    bindings_.erase(name);
}

void EventManager::SetFocus(const bool& focus) { has_focus_ = focus; }

void EventManager::SetCurrentState(const StateType& state)
{
    current_state_ = state;
}

void EventManager::ProcessDiscreteEvent(sf::Event& event)
{
    for (auto& binding_itr : bindings_) {
        Binding* bind = binding_itr.second.get();

        for (auto& event_itr : bind->events_) {
            const auto kSfmlEvent = static_cast<EventType>(event.type);

            if (event_itr.first != kSfmlEvent) {
                continue;
            }

            const auto kBoundEventInfo = event_itr.second;

            switch (kSfmlEvent) {
            case EventType::kKeyDown:
            case EventType::kKeyUp:
                if (kBoundEventInfo.key_code == event.key.code) {
                    if (bind->details_.key_code_ == -1) {
                        bind->details_.key_code_ = kBoundEventInfo.key_code;
                    }
                    ++bind->event_matches_;
                }
                break;
            case EventType::kMouseButtonDown:
            case EventType::kMouseButtonUp:
                if (kBoundEventInfo.key_code == event.mouseButton.button) {
                    bind->details_.mouse_position_ = { event.mouseButton.x, event.mouseButton.y };
                    if (bind->details_.key_code_ == -1) {
                        bind->details_.key_code_ = kBoundEventInfo.key_code;
                    }
                    ++bind->event_matches_;
                }
                break;
            case EventType::kMouseMoved:
                bind->details_.mouse_position_ = { event.mouseMove.x, event.mouseMove.y };
                ++bind->event_matches_;
                break;
            case EventType::kMouseWheel:
                bind->details_.mouse_wheel_delta_ = event.mouseWheel.delta;
                ++bind->event_matches_;
                break;
            case EventType::kWindowResized:
                bind->details_.size_.x = event.size.width;
                bind->details_.size_.y = event.size.height;
                ++bind->event_matches_;
                break;
            case EventType::kTextEntered:
                bind->details_.text_entered_ = event.text.unicode;
                ++bind->event_matches_;
                break;
            default:
                ++bind->event_matches_;
                continue;
            }
            break;
        }
    }
}

void EventManager::ProcessContinuousEventsAndUpdate()
{
    if (!has_focus_) {
        return;
    }

    for (auto& binding_itr : bindings_) {
        Binding* bind = binding_itr.second.get();

        for (auto event_itr : bind->events_) {

            const auto kBoundEventInfo = event_itr.second;

            switch (event_itr.first) {
            case (EventType::kKeyboard):
                if (sf::Keyboard::isKeyPressed(
                        sf::Keyboard::Key(kBoundEventInfo.key_code))) {
                    if (bind->details_.key_code_ == -1) {
                        bind->details_.key_code_ = kBoundEventInfo.key_code;
                    }
                    ++(bind->event_matches_);
                }
                break;
            case (EventType::kMouse):
                if (sf::Mouse::isButtonPressed(
                        sf::Mouse::Button(kBoundEventInfo.key_code))) {
                    if (bind->details_.key_code_ == -1) {
                        bind->details_.key_code_ = kBoundEventInfo.key_code;
                    }
                    ++(bind->event_matches_);
                }
                break;
            case (EventType::kJoystick):
                // Up for expansion.
                break;
            default:
                // Error?
                break;
            }
        }

        if (bind->events_.size() == bind->event_matches_) {
            auto state_callbacks = callbacks_.find(current_state_);
            auto global_callbacks = callbacks_.find(StateType(0));

            if (state_callbacks != callbacks_.end()) {
                auto call_itr = state_callbacks->second.find(bind->name_);
                if (call_itr != state_callbacks->second.end()) {
                    call_itr->second(&bind->details_);
                }
            }

            if (global_callbacks != callbacks_.end()) {
                auto call_itr = global_callbacks->second.find(bind->name_);
                if (call_itr != global_callbacks->second.end()) {
                    call_itr->second(&bind->details_);
                }
            }
        }
        bind->event_matches_ = 0;
        bind->details_.Clear();
    }
}

void EventManager::LoadBindings()
{
    bindings_.clear();
    std::string delimiter = ":";
    std::ifstream bindings;

    bindings.open("./resources/keys.cfg");
    if (!bindings.is_open()) {
        std::cout << "! Failed loading keys.cfg." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(bindings, line)) {
        std::stringstream keystream(line);
        std::string callback_name;
        keystream >> callback_name;
        auto bind = std::make_unique<Binding>(callback_name);

        while (!keystream.eof()) {
            std::string keyval;
            keystream >> keyval;

            const size_t kEnd = keyval.find(delimiter);
            const EventType kType = EventType(stoi(keyval.substr(0, kEnd)));
            const int kCode = stoi(
                keyval.substr(kEnd + delimiter.length(),
                    keyval.find(delimiter, kEnd + delimiter.length())));
            BoundEventInfo event_info;
            event_info.key_code = kCode;

            bind->BindEvent(kType, event_info);
        }

        AddBinding(std::move(bind));
    }
    bindings.close();
}
