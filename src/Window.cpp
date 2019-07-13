#include "Window.h"

// Public

Window::Window() { Setup("Title", sf::Vector2u(640, 480)); }

Window::Window(const std::string& title, const sf::Vector2u& size)
{
    Setup(title, size);
}

Window::~Window() { Destroy(); }

void Window::BeginDraw() { window_.clear(); }

void Window::EndDraw() { window_.display(); }

void Window::Draw(const sf::Drawable& drawable_object)
{
    window_.draw(drawable_object);
}

void Window::Update()
{
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::LostFocus) {
            is_focused_ = false;
            event_manager_.SetFocus(false);
        } else if (event.type == sf::Event::GainedFocus) {
            is_focused_ = true;
            event_manager_.SetFocus(true);
        }
        event_manager_.ProcessDiscreteEvent(event);
    }
    event_manager_.ProcessContinuousEventsAndUpdate();
}

void Window::Close(const EventDetails* const) { is_done_ = true; }

void Window::ToggleFullscreen(const EventDetails* const)
{
    is_fullscreen_ = !is_fullscreen_;
    Destroy();
    Create();
}

EventManager* Window::GetEventManager() { return &event_manager_; }

sf::RenderWindow* Window::GetRenderWindow() { return &window_; }

const sf::FloatRect Window::GetViewSpace() const
{
    sf::Vector2f origin = window_.getView().getCenter() - window_.getView().getSize() / 2.0f;
    return sf::FloatRect(origin, window_.getView().getSize());
}

// Private

void Window::Setup(const std::string& title, const sf::Vector2u& size)
{
    window_title_ = title;
    window_size_ = size;
    is_focused_ = true;
    is_fullscreen_ = false;
    is_done_ = false;

    event_manager_.AddCallback(StateType(0), "Fullscreen_toggle",
        &Window::ToggleFullscreen, this);
    event_manager_.AddCallback(StateType(0), "Window_close", &Window::Close,
        this);

    Create();
}

void Window::Create()
{
    auto style = (is_fullscreen_ ? sf::Style::Fullscreen : sf::Style::Default);
    window_.create({ window_size_.x, window_size_.y, 32 }, window_title_, style);
}

void Window::Destroy() { window_.close(); }
