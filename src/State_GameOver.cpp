#include "State_GameOver.h"
#include "StateManager.h"

State_GameOver::State_GameOver(StateManager* state_manager)
    : BaseState(state_manager)
{
}

State_GameOver::~State_GameOver() {}

void State_GameOver::OnCreate()
{
    elapsed_time_ = 0;
    font_.loadFromFile(Utils::GetExecutableDirectory() + "resources/media/fonts/arial.ttf");
    text_.setFont(font_);
    text_.setCharacterSize(16);
    text_.setString("You beat the game! Congratulations!");
    text_.setFillColor(sf::Color::White);
    text_.setOrigin(text_.getLocalBounds().width / 2,
        text_.getLocalBounds().height / 2);
    text_.setPosition(400, 300);

    state_manager_->Remove(StateType::kGame);
}

void State_GameOver::OnDestroy() {}

void State_GameOver::Activate() {}
void State_GameOver::Deactivate() {}

void State_GameOver::Update(const sf::Time& time)
{
    elapsed_time_ += time.asSeconds();
    if (elapsed_time_ >= 5.0f) {
        state_manager_->Remove(StateType::kGameOver);
        state_manager_->SwitchTo(StateType::kMainMenu);
    }
}

void State_GameOver::Draw() const
{
    sf::RenderWindow* window = state_manager_->GetContext()->window_->GetRenderWindow();
    window->draw(text_);
}
