#ifndef STATE_GAMEOVER_H
#define STATE_GAMEOVER_H

#include "BaseState.h"
#include <SFML/Graphics.hpp>

class State_GameOver : public BaseState {
public:
    State_GameOver(StateManager* state_manager_);
    ~State_GameOver();

    void OnCreate();
    void OnDestroy();

    void Activate();
    void Deactivate();

    void Update(const sf::Time& time_);
    void Draw() const;

private:
    sf::Font font_;
    sf::Text text_;
    float elapsed_time_;
};

#endif // !STATE_GAMEOVER_H
