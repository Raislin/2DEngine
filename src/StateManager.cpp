#include "StateManager.h"
#include "State_Game.h"
#include "State_GameOver.h"
#include "State_Intro.h"
#include "State_MainMenu.h"
#include "State_Pause.h"

StateManager::StateManager(SharedContext* context)
    : shared_(context)
{
    RegisterState<State_Intro>(StateType::kIntro);
    RegisterState<State_MainMenu>(StateType::kMainMenu);
    RegisterState<State_Game>(StateType::kGame);
    RegisterState<State_Pause>(StateType::kPaused);
    RegisterState<State_GameOver>(StateType::kGameOver);
}

StateManager::~StateManager()
{
    for (auto& itr : states_) {
        itr.second->OnDestroy();
    }
}

// My version
void StateManager::Draw() const
{
    auto first_opaque_from_end = std::find_if(
        states_.crbegin(), states_.crend(),
        [](const std::pair<StateType, std::unique_ptr<BaseState>>& i) {
            return !i.second->IsTransparent() ? true : false;
        });

    if (first_opaque_from_end == states_.crend()) {
        std::for_each(
            states_.cbegin(), states_.cend(),
            [](const std::pair<StateType, std::unique_ptr<BaseState>>& i) {
                i.second->Draw();
            });
    } else {
        std::for_each(
            std::prev(first_opaque_from_end.base()), states_.cend(),
            [&](const std::pair<StateType, std::unique_ptr<BaseState>>& i) {
                shared_->window_->GetRenderWindow()->setView(i.second->GetView());
                i.second->Draw();
            });
    }
}

void StateManager::ProcessRequests()
{
    for (auto& i : to_remove_)
        RemoveState(i);
    to_remove_.clear();
}

void StateManager::Update(const sf::Time& time)
{
    auto first_unblocked_state = std::find_if(
        states_.crbegin(), states_.crend(),
        [](const std::pair<StateType, std::unique_ptr<BaseState>>& i) {
            return (!i.second->IsTranscendent() ? true : false);
        });

    if (first_unblocked_state == states_.crend()) {
        std::for_each(
            states_.cbegin(), states_.cend(),
            [&](const std::pair<StateType, std::unique_ptr<BaseState>>& i) {
                i.second->Update(time);
            });
    } else {
        std::for_each(
            std::prev(first_unblocked_state.base()), states_.cend(),
            [&](const std::pair<StateType, std::unique_ptr<BaseState>>& i) {
                i.second->Update(time);
            });
    }
}

SharedContext* StateManager::GetContext() { return shared_; }

bool StateManager::HasState(const StateType& type) const
{
    for (auto& t : states_) {
        if (t.first == type) {
            auto removed = std::find(to_remove_.begin(), to_remove_.end(), type);
            if (removed == to_remove_.end())
                return true;
            else
                return false;
        }
    }
    return false;
}

void StateManager::SwitchTo(const StateType& type)
{
    shared_->event_manager_->SetCurrentState(type);
    auto found = std::find_if(
        states_.begin(), states_.end(),
        [&](const auto& i) {
            return i.first == type;
        });

    if (!states_.empty()) {
        states_.back().second->Deactivate();
    }

    if (found == states_.end()) {
        CreateState(type);
    } else {
        std::rotate(found, found + 1, states_.end());
    }
    states_.back().second->Activate();
    shared_->window_->GetRenderWindow()->setView(states_.back().second->GetView());
}

void StateManager::Remove(const StateType& type) { to_remove_.push_back(type); }

void StateManager::CreateState(const StateType& type)
{
    auto new_state = state_factory_.find(type);
    if (new_state == state_factory_.end())
        return;
    auto state = new_state->second();
    state->SetView(shared_->window_->GetRenderWindow()->getDefaultView());
    state->OnCreate();
    states_.emplace_back(type, std::move(state));
}

void StateManager::RemoveState(const StateType& type)
{
    auto found = std::find_if(
        states_.begin(), states_.end(),
        [&](const auto& i) {
            return i.first == type;
        });

    if (found != states_.end()) {
        found->second->OnDestroy();
        states_.erase(found);
    }
}
