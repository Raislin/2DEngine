#ifndef STATEMANAGER_H
#define STATEMANAGER_H
#include "BaseState.h"
#include "SharedContext.h"
#include <memory>
#include <unordered_map>
#include <vector>

using StateContainer = std::vector<std::pair<StateType, std::unique_ptr<BaseState>>>;
using TypeContainer = std::vector<StateType>;
using StateFactory = std::unordered_map<StateType, std::function<std::unique_ptr<BaseState>()>>;

class StateManager {
public:
    StateManager(SharedContext* context);
    ~StateManager();

    void Update(const sf::Time& time);
    void Draw() const;

    void ProcessRequests();

    SharedContext* GetContext();
    bool HasState(const StateType& type) const;

    void SwitchTo(const StateType& type);
    void Remove(const StateType& type);

private:
    void CreateState(const StateType& type);
    void RemoveState(const StateType& type);

    template <class T>
    void RegisterState(const StateType& type)
    {
        state_factory_[type] = [this]() -> std::unique_ptr<BaseState> {
            return std::make_unique<T>(this);
        };
    }

    SharedContext* shared_;
    StateContainer states_;
    TypeContainer to_remove_;
    StateFactory state_factory_;
};

#endif // STATEMANAGER_H
