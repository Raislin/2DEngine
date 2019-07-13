#ifndef DEBUG_OVERLAY_H
#define DEBUG_OVERLAY_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class DebugOverlay {
public:
    DebugOverlay()
    {
        debug_ = false;
    }

    void Add(std::unique_ptr<sf::Drawable> drawable)
    {
        drawables_.push_back(std::move(drawable));
    }

    void Draw(sf::RenderWindow* window)
    {
        for (auto& i : drawables_) {
            window->draw(*i);
        }
        drawables_.clear();
    }

    bool Debug() { return debug_; }
    void SetDebug(const bool& val) { debug_ = val; }

private:
    std::vector<std ::unique_ptr<sf::Drawable>> drawables_;
    bool debug_;
};

#endif // !DEBUG_OVERLAY_H
