#include "State_Intro.h"
#include "StateManager.h"
#include "Utilities.h"
#include <iostream>

State_Intro::State_Intro(StateManager* state_manager)
    : BaseState(state_manager)
{
}

State_Intro::~State_Intro() {}

void State_Intro::OnCreate()
{
    time_passed_ = 0.0f;

    sf::Vector2u window_size_ = state_manager_->GetContext()->window_->GetRenderWindow()->getSize();

    texture_ = state_manager_->GetContext()->texture_manager_->GetResource("intro");
    sprite_.setTexture(*texture_);
    sprite_.setOrigin(texture_->getSize().x / 2.0f, texture_->getSize().y / 2.0f);

    sprite_.setPosition(window_size_.x / 2.0f, 0);

    font_.loadFromFile(Utils::GetExecutableDirectory() + "resources/media/fonts/arial.ttf");
    text_.setFont(font_);
    text_.setString({ "Press SPACE to continue" });
    text_.setCharacterSize(15);
    sf::FloatRect textRect = text_.getLocalBounds();
    text_.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    text_.setPosition(window_size_.x / 2.0f, window_size_.y / 2.0f);

    state_manager_->GetContext()->event_manager_->AddCallback(
        StateType::kIntro, "Intro_Continue", &State_Intro::Continue, this);
    state_manager_->GetContext()->event_manager_->AddCallback(
        StateType::kIntro, "Key_Up", &State_Intro::Troll, this);
}

void State_Intro::OnDestroy()
{
    state_manager_->GetContext()->event_manager_->RemoveCallback(StateType::kIntro,
        "Intro_Continue");
    state_manager_->GetContext()->event_manager_->RemoveCallback(StateType::kIntro,
        "Key_Up");
}

void State_Intro::Update(const sf::Time& time)
{
    if (time_passed_ < 1.0f) { // Less than five seconds.
        time_passed_ += time.asSeconds();
        sprite_.setPosition(sprite_.getPosition().x,
            sprite_.getPosition().y + (240 * time.asSeconds()));
    } else {
        if (sprite_.getPosition().y < -static_cast<int>(texture_->getSize().y / 2)) {
            text_.setString({ "OK..." });
            sf::FloatRect textRect = text_.getLocalBounds();
            text_.setOrigin(textRect.left + textRect.width / 2.0f,
                textRect.top + textRect.height / 2.0f);
            text_.setPosition(
                state_manager_->GetContext()->window_->GetRenderWindow()->getSize().x / 2.0f,
                state_manager_->GetContext()->window_->GetRenderWindow()->getSize().y / 2.0f);
        }
    }
}

void State_Intro::Draw() const
{
    sf::RenderWindow* window = state_manager_->GetContext()->window_->GetRenderWindow();

    window->draw(sprite_);
    if (time_passed_ >= 1.0f) {
        window->draw(text_);
    }
}

void State_Intro::Continue(const EventDetails* const)
{
    if (time_passed_ >= 1.0f) {
        state_manager_->SwitchTo(StateType::kMainMenu);
        state_manager_->Remove(StateType::kIntro);
    }
}

void State_Intro::Troll(const EventDetails* const)
{
    sprite_.setPosition(sprite_.getPosition().x, sprite_.getPosition().y - 1);
}

void State_Intro::Activate() {}
void State_Intro::Deactivate() {}
