#ifndef State_MainMenu_H
#define State_MainMenu_H

#include "BaseState.h"
#include "EventManager.h"
#include <array>

class State_MainMenu : public BaseState {
public:
  State_MainMenu(StateManager *state_manager);
  ~State_MainMenu();

  void OnCreate();
  void OnDestroy();

  void Activate();
  void Deactivate();

  void Update(const sf::Time &time);
  void Draw() const;
  void MouseClick(const EventDetails *const details);

private:
  sf::Font font_;
  sf::Text title_text_;

  sf::Vector2f button_size_;
  sf::Vector2f first_button_position_;
  unsigned int button_padding_;

  std::array<sf::RectangleShape, 3> button_rectangles_;
  std::array<sf::Text, 3> labels_;
};

#endif // BASESTATE_H
