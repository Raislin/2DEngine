#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H

#include "BaseAnimation.h"
#include "Directions.h"
#include "TextureManager.h"
#include "Types.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <unordered_map>

using Animations = std::unordered_map<std::string, std::unique_ptr<BaseAnimation>>;

class SpriteSheet {
public:
    SpriteSheet(TextureManager* texture_manager);
    ~SpriteSheet();
    void CropSprite(const sf::Rect<int>& rectangle);
    bool LoadSheet(const FilePath& file);
    const BaseAnimation* GetCurrentAnimation() const;
    bool SetAnimation(const std::string& animation_id, const bool& play = false,
        const bool& loop = false);
    void Update(const float& time_delta);
    void Draw(sf::RenderWindow* window);

    void SetSpriteSize(const sf::Vector2i& size);
    void SetSpritePosition(const sf::Vector2f& position);
    void SetDirection(const Direction& direction);

    const sf::Vector2i& GetSpriteSize() const;
    const Direction& GetDirection() const;

private:
    std::shared_ptr<const sf::Texture> texture_;
    std::string texture_id_;
    sf::Sprite sprite_;
    sf::Vector2i sprite_size_;
    sf::Vector2f sprite_scale_ = { 1.0f, 1.0f };
    Direction direction_ = Direction::kRight;
    std::string animation_type_;
    Animations animations_map_;
    std::unique_ptr<BaseAnimation> current_animation_;
    TextureManager* texture_manager_;
};

#endif // !SPRITE_SHEET_H
