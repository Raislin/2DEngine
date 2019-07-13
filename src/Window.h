#ifndef HEARTHWELL_WINDOW_H_
#define HEARTHWELL_WINDOW_H_

#include "EventManager.h"
#include <SFML/Graphics.hpp>

class Window {
public:
    Window();
    Window(const std::string& title, const sf::Vector2u& size);
    ~Window();

    void BeginDraw();
    void EndDraw();
    void Draw(const sf::Drawable& drawable_object);
    void Update();
    void ToggleFullscreen(const EventDetails* const details);
    void Close(const EventDetails* const details = nullptr);

    bool IsDone() const;
    bool IsFullscreen() const;
    bool IsFocused() const;

    EventManager* GetEventManager();
    sf::RenderWindow* GetRenderWindow();
    sf::Vector2u GetWindowSize() const;
    const sf::FloatRect GetViewSpace() const;

private:
    void Setup(const std::string& title, const sf::Vector2u& size);
    void Create();
    void Destroy();

    EventManager event_manager_;
    sf::RenderWindow window_;
    std::string window_title_;
    sf::Vector2u window_size_;
    bool is_fullscreen_;
    bool is_done_;
    bool is_focused_;
};

inline bool Window::IsDone() const { return is_done_; }

inline bool Window::IsFullscreen() const { return is_fullscreen_; }

inline bool Window::IsFocused() const { return is_focused_; }

inline sf::Vector2u Window::GetWindowSize() const { return window_size_; }

#endif
