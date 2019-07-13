#ifndef GAME_H
#define GAME_H
#include "EntityManager.h"
#include "StateManager.h"
#include "TextureManager.h"
#include "Window.h"

class Game {
public:
    Game();
    ~Game();

    void Update();
    void Render();
    void LateUpdate();

    Window* GetWindow();

    sf::Time GetElapsed();
    void RestartClock();

private:
    SharedContext context_;
    Window window_;
    StateManager state_manager_;
    EntityManager entity_manager_;
    TextureManager texture_manager_;
    sf::Clock clock_;
    sf::Time elapsed_;
    unsigned counter_ = 0;
    const float kminimum_frame_time_ = 1.0f / 3600.0f;

    sf::Vector2i increment_;
};
#endif // GAME_H
