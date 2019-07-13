#include "State_MainMenu.h"
#include "StateManager.h"
#include <string>

State_MainMenu::State_MainMenu(StateManager* state_manager)
    : BaseState(state_manager)
{
}

State_MainMenu::~State_MainMenu() {}

void State_MainMenu::OnCreate()
{
    font_.loadFromFile(Utils::GetExecutableDirectory() + "resources/media/fonts/arial.ttf");
    title_text_.setFont(font_);
    title_text_.setString({ "Main Menu" });
    title_text_.setCharacterSize(18);

    sf::FloatRect title_rect = title_text_.getLocalBounds();
    title_text_.setOrigin(title_rect.left + title_rect.width / 2.0f,
        title_rect.top + title_rect.height / 2.0f);
    title_text_.setPosition(400.0f, 100.0f);

    button_size_ = sf::Vector2f(300.0f, 32.0f);
    first_button_position_ = sf::Vector2f(400.0f, 200.0f);
    button_padding_ = 4;

    const std::array<std::string, 3> kLabels{ "PLAY", "CREDITS", "EXIT" };

    for (size_t i = 0; i < kLabels.size(); ++i) {
        sf::Vector2f button_position(first_button_position_.x,
            first_button_position_.y + (i * (button_size_.y + button_padding_)));
        button_rectangles_[i].setSize(button_size_);
        button_rectangles_[i].setFillColor(sf::Color::Red);

        button_rectangles_[i].setOrigin(button_size_.x / 2.0f,
            button_size_.y / 2.0f);
        button_rectangles_[i].setPosition(button_position);

        labels_[i].setFont(font_);
        labels_[i].setString(kLabels[i]);
        labels_[i].setCharacterSize(12);

        sf::FloatRect label_rectangle = labels_[i].getLocalBounds();
        labels_[i].setOrigin(label_rectangle.left + label_rectangle.width / 2.0f,
            label_rectangle.top + label_rectangle.height / 2.0f);

        labels_[i].setPosition(button_position);
    }

    state_manager_->GetContext()->event_manager_->AddCallback(
        StateType::kMainMenu, "Mouse_Left", &State_MainMenu::MouseClick, this);
}

void State_MainMenu::OnDestroy()
{
    state_manager_->GetContext()->event_manager_->RemoveCallback(
        StateType::kMainMenu, "Mouse_Left");
}

void State_MainMenu::Activate()
{
    if (state_manager_->HasState(StateType::kGame) && labels_[0].getString() == "PLAY") {
        labels_[0].setString({ "RESUME" });
        sf::FloatRect label_rectangle = labels_[0].getLocalBounds();
        labels_[0].setOrigin(label_rectangle.left + label_rectangle.width / 2.0f,
            label_rectangle.top + label_rectangle.height / 2.0f);
    }
}

void State_MainMenu::Deactivate() {}

void State_MainMenu::Update(const sf::Time&) {}

void State_MainMenu::Draw() const
{
    sf::RenderWindow* render_window = state_manager_->GetContext()->window_->GetRenderWindow();

    render_window->draw(title_text_);
    for (const auto& button : button_rectangles_)
        render_window->draw(button);
    for (const auto& label : labels_)
        render_window->draw(label);
}

void State_MainMenu::MouseClick(const EventDetails* const details)
{
    sf::Vector2i mouse_position = details->mouse_position_;

    float half_button_width = button_size_.x / 2.0f;
    float half_button_height = button_size_.y / 2.0f;

    for (size_t i = 0; i < button_rectangles_.size(); ++i) {
        if (mouse_position.x >= button_rectangles_[i].getPosition().x - half_button_width && mouse_position.x <= button_rectangles_[i].getPosition().x + half_button_width && mouse_position.y >= button_rectangles_[i].getPosition().y - half_button_height && mouse_position.y <= button_rectangles_[i].getPosition().y + half_button_height) {
            switch (i) {
            case 0:
                state_manager_->SwitchTo(StateType::kGame);
                state_manager_->Remove(StateType::kMainMenu);
                break;
            case 1:
                // credits state
                break;
            case 2:
                state_manager_->GetContext()->window_->Close();
                break;
            default:;
                // error
            }
        }
    }
}
