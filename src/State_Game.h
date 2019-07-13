#ifndef STATE_GAME_H
#define STATE_GAME_H

#include "BaseState.h"
#include "EventManager.h"
#include "Map.h"
#include <memory>

class State_Game : public BaseState {
public:
    State_Game(StateManager* state_manager);
    ~State_Game();

    void OnCreate();
    void OnDestroy();

    void Activate();
    void Deactivate();

    void Update(const sf::Time& time);
    void Draw() const;

    void MainMenu(const EventDetails* const details);
    void Pause(const EventDetails* const details);
    void ToggleOverlay(const EventDetails* const details);

private:
    std::unique_ptr<Map> game_map_;
};

#endif
