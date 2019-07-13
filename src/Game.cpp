#include "Game.h"
#include <iostream>

Game::Game()
    : window_("Chapter 2", sf::Vector2u(800, 600))
    , state_manager_(&context_)
    , entity_manager_(&context_, 100)
{
    RestartClock();
    srand(static_cast<unsigned>(time(nullptr)));
    // Setting up class members.

    context_.window_ = &window_;
    context_.event_manager_ = window_.GetEventManager();
    context_.texture_manager_ = &texture_manager_;
    context_.entity_manager_ = &entity_manager_;
    state_manager_.SwitchTo(StateType::kIntro);
}

Game::~Game() {}

sf::Time Game::GetElapsed() { return elapsed_; }
void Game::RestartClock()
{
    ++counter_;
    elapsed_ = clock_.restart();
    // Set frame rate limit
    if (elapsed_.asSeconds() < kminimum_frame_time_) {
      elapsed_ = sf::seconds(kminimum_frame_time_);
    }
    if (counter_ == 10000)
    {
      std::cout << 1.0f / elapsed_.asSeconds() << '\n';
      counter_ = 0;
    }
}
Window* Game::GetWindow() { return &window_; }

void Game::Update()
{
    window_.Update();
    state_manager_.Update(elapsed_);
    texture_manager_.Update();
}

void Game::Render()
{
    window_.BeginDraw(); // Clear.
    state_manager_.Draw();
    if (context_.debug_overlay_.Debug()) {
        context_.debug_overlay_.Draw(context_.window_->GetRenderWindow());
    }
    window_.EndDraw(); // Display.
}

void Game::LateUpdate()
{
    state_manager_.ProcessRequests();
    RestartClock();
}
