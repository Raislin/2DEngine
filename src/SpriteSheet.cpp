#include "SpriteSheet.h"
#include "DirectionalAnimation.h"
#include "Utilities.h"

SpriteSheet::SpriteSheet(TextureManager* texture_manager)
    : texture_manager_(texture_manager)
{
}

SpriteSheet::~SpriteSheet() {}

void SpriteSheet::CropSprite(const sf::Rect<int>& rectangle)
{
    sprite_.setTextureRect(rectangle);
}

bool SpriteSheet::LoadSheet(const FilePath& file)
{
    std::ifstream sheet;
    sheet.open(Utils::GetExecutableDirectory() + file);
    if (sheet.is_open()) {
        animations_map_.clear();
        std::string line;

        while (std::getline(sheet, line)) {
            if (line[0] == '|') { // Ignore comment
                continue;
            }

            std::stringstream keystream(line);
            std::string type;
            keystream >> type;

            if (type == "Texture") {
                if (texture_id_ != "") {
                    std::cerr << "! Duplicate texture entries in: " << file << std::endl;
                    continue;
                }

                std::string texture_id;
                keystream >> texture_id;
                texture_ = texture_manager_->GetResource(texture_id);

                if (!texture_) {
                    std::cerr << "! Could not set up the texture: " << texture_id
                              << std::endl;
                    continue;
                }

                texture_id_ = texture_id;
                sprite_.setTexture(*texture_);
            }

            else if (type == "Size") {
                keystream >> sprite_size_.x >> sprite_size_.y;
                SetSpriteSize(sprite_size_);
            } else if (type == "Scale") {
                keystream >> sprite_scale_.x >> sprite_scale_.y;
                sprite_.setScale(sprite_scale_);
            } else if (type == "AnimationType") {
                keystream >> animation_type_;
            } else if (type == "Animation") {
                std::string animation_id;
                keystream >> animation_id;

                if (animations_map_.find(animation_id) != animations_map_.end()) {
                    std::cerr << "! Duplicate animation(" << animation_id
                              << ") in: " << file << std::endl;
                    continue;
                }

                std::unique_ptr<BaseAnimation> animation;

                if (animation_type_ == "Directional") {
                    animation = std::make_unique<DirectionalAnimation>();
                } else {
                    std::cerr << "! Unknown animation type: " << animation_type_
                              << std::endl;
                    continue;
                }

                keystream >> *animation;
                animation->SetSpriteSheet(this);
                animation->SetId(animation_id);
                animation->Reset();

                if (!current_animation_) {
                    current_animation_ = std::move(animation);
                    current_animation_->Play();
                } else {
                    animations_map_.emplace(animation_id, std::move(animation));
                }
            }
        }
        sheet.close();
        return true;
    }
    std::cerr << "! Failed loading spritesheet: " << file << std::endl;
    return false;
}

const BaseAnimation* SpriteSheet::GetCurrentAnimation() const
{
    return current_animation_.get();
}

bool SpriteSheet::SetAnimation(const std::string& animation_id,
    const bool& play, const bool& loop)
{
    auto itr = animations_map_.find(animation_id);
    if (itr == animations_map_.end()) {
        return false;
    }
    if (itr->second == current_animation_) {
        return false;
    }
    if (current_animation_) {
        current_animation_->Stop();
        animations_map_.emplace(current_animation_->animation_id_,
            std::move(current_animation_));
    }
    current_animation_ = std::move(itr->second);
    animations_map_.erase(itr);
    current_animation_->SetLooping(loop);
    if (play) {
        current_animation_->Play();
    }
    current_animation_->CropSprite();
    return true;
}

void SpriteSheet::Update(const float& time_delta)
{
    current_animation_->Update(time_delta);
}

void SpriteSheet::Draw(sf::RenderWindow* window) { window->draw(sprite_); }

void SpriteSheet::SetSpriteSize(const sf::Vector2i& size)
{
    sprite_size_ = size;
    sprite_.setOrigin(sprite_size_.x / 2.0f, static_cast<float>(sprite_size_.y));
}

void SpriteSheet::SetSpritePosition(const sf::Vector2f& position)
{
    sprite_.setPosition(position);
}

void SpriteSheet::SetDirection(const Direction& direction)
{
    if (direction != direction_) {
        direction_ = direction;
        current_animation_->CropSprite();
    }
}

const sf::Vector2i& SpriteSheet::GetSpriteSize() const { return sprite_size_; }

const Direction& SpriteSheet::GetDirection() const { return direction_; }
