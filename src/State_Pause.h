#ifndef STATE_PAUSE_H
#define STATE_PAUSE_H

#include "BaseState.h"
#include "EventManager.h"

class State_Pause : public BaseState {
public:
  State_Pause(StateManager *state_manager);
  ~State_Pause();

  void OnCreate();
  void OnDestroy();

  void Activate();
  void Deactivate();

  void Update(const sf::Time &time);
  void Draw() const;

  void Unpause(const EventDetails *const details);

private:
  sf::Font font_;
  sf::Text text_;
  sf::RectangleShape screen_dimmer_;
};

#endif
