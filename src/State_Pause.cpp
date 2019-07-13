#include "State_Pause.h"
#include "StateManager.h"

State_Pause::State_Pause(StateManager* state_manager)
    : BaseState(state_manager)
{
}

State_Pause::~State_Pause() {}

void State_Pause::OnCreate()
{
    SetTransparent(true);

    font_.loadFromFile(Utils::GetExecutableDirectory() + "resources/media/fonts/arial.ttf");
    text_.setFont(font_);
    text_.setString({ "PAUSED" });
    text_.setCharacterSize(14);
    text_.setStyle(sf::Text::Bold);

    const sf::FloatRect text_rectangle = text_.getLocalBounds();
    text_.setOrigin(text_rectangle.left + text_rectangle.width / 2.0f,
        text_rectangle.top + text_rectangle.height / 2.0f);

    const sf::Vector2u window_size = state_manager_->GetContext()->window_->GetWindowSize();
    text_.setPosition(sf::Vector2f(window_size / 2u));

    screen_dimmer_.setSize(sf::Vector2f(window_size));
    screen_dimmer_.setPosition(0.0f, 0.0f);
    screen_dimmer_.setFillColor(sf::Color(0, 0, 0, 150));

    state_manager_->GetContext()->event_manager_->AddCallback(
        StateType::kPaused, "Key_Pause", &State_Pause::Unpause, this);
}

void State_Pause::OnDestroy()
{
    state_manager_->GetContext()->event_manager_->RemoveCallback(
        StateType::kPaused, "Key_Pause");
}

void State_Pause::Activate() {}

void State_Pause::Deactivate() {}

void State_Pause::Update(const sf::Time&) {}

void State_Pause::Draw() const
{
    state_manager_->GetContext()->window_->Draw(text_);
    state_manager_->GetContext()->window_->Draw(screen_dimmer_);
}

void State_Pause::Unpause(const EventDetails* const)
{
    state_manager_->SwitchTo(StateType::kGame);
}
