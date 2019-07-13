#ifndef STATE_INTRO_H
#define STATE_INTRO_H

#include "BaseState.h"
#include "EventManager.h"
#include <memory>

class State_Intro : public BaseState {
public:
    State_Intro(StateManager* state_manager);
    ~State_Intro();

    void OnCreate();
    void OnDestroy();

    void Activate();
    void Deactivate();

    void Update(const sf::Time& time);
    void Draw() const;

    void Continue(const EventDetails* const details);
    void Troll(const EventDetails* const details);

private:
    std::shared_ptr<const sf::Texture> texture_;
    sf::Sprite sprite_;
    sf::Font font_;
    sf::Text text_;

    float time_passed_;
};

#endif
