#include "BaseEntity.h"
#include "EntityManager.h"
#include "Map.h"
#include "SharedContext.h"
#include <cmath>

bool SortCollisions(const CollisionElement& one, const CollisionElement& two)
{
    return one.area_ > two.area_;
}

BaseEntity::BaseEntity(EntityManager* entity_manager)
    : entity_manager_(entity_manager)
{
}

BaseEntity::~BaseEntity() {}

void BaseEntity::SetPosition(const sf::Vector2f pos)
{
    position_ = pos;
    UpdateAABB();
}

void BaseEntity::SetSize(const sf::Vector2f size)
{
    size_ = size;
    UpdateAABB();
}

void BaseEntity::SetAcceleration(const sf::Vector2f acceleration)
{
    acceleration_ = acceleration;
}

void BaseEntity::SetState(const EntityState& state)
{
    if (state_ == EntityState::Dying) {
        return;
    }
    state_ = state;
}

const sf::Vector2f& BaseEntity::GetSize() const { return size_; }
std::string BaseEntity::GetName() const { return name_; }
EntityState BaseEntity::GetState() const { return state_; }
EntityID BaseEntity::GetId() const { return id_; }
EntityType BaseEntity::GetType() const { return type_; }
const sf::Vector2f& BaseEntity::GetPosition() const { return position_; }

void BaseEntity::Move(const sf::Vector2f distance)
{
    old_position_ = position_;
    position_ += distance;
    auto map_size = entity_manager_->GetContext()->game_map_->GetMapSize();
    if (position_.x < 0) {
        position_.x = 0;
    } else if (position_.x > static_cast<float>((map_size.x + 1) * Sheet::Tile_Size)) {
        position_.x = static_cast<float>((map_size.x + 1) * Sheet::Tile_Size);
    }

    if (position_.y < 0) {
        position_.y = 0;
    } else if (position_.y > static_cast<float>((map_size.y + 1) * Sheet::Tile_Size)) {
        position_.y = static_cast<float>((map_size.y + 1) * Sheet::Tile_Size);
        SetState(EntityState::Dying);
    }

    UpdateAABB();
}

void BaseEntity::AddVelocity(const sf::Vector2f velocity)
{
    velocity_ += velocity;
    if (std::abs(velocity_.x) > max_velocity_.x) {
        if (velocity_.x < 0) {
            velocity_.x = -max_velocity_.x;
        } else {
            velocity_.x = max_velocity_.x;
        }
    }

    if (std::abs(velocity_.y) > max_velocity_.y) {
        if (velocity_.y < 0) {
            velocity_.y = -max_velocity_.y;
        } else {
            velocity_.y = max_velocity_.y;
        }
    }
}

void BaseEntity::Accelerate(sf::Vector2f acceleration)
{
    acceleration_ += acceleration;
}

void BaseEntity::ApplyFriction(const sf::Vector2f friction)
{
    if (velocity_.x != 0) {
        if (std::abs(velocity_.x) - std::abs(friction.x) < 0) {
            velocity_.x = 0;
        } else {
            if (velocity_.x < 0) {
                velocity_.x += friction.x;
            } else {
                velocity_.x -= friction.x;
            }
        }
    }

    if (velocity_.y != 0) {
        if (std::abs(velocity_.y) - std::abs(friction.y) < 0) {
            velocity_.y = 0;
        } else {
            if (velocity_.y < 0) {
                velocity_.y += friction.y;
            } else {
                velocity_.y -= friction.y;
            }
        }
    }
}

void BaseEntity::Update(float delta_time)
{
    Map* map = entity_manager_->GetContext()->game_map_;
    float gravity = map->GetGravity();
    Accelerate({ 0, gravity });
    AddVelocity({ acceleration_.x * delta_time, acceleration_.y * delta_time });
    SetAcceleration({ 0.0f, 0.0f });
    sf::Vector2f friction_value;
    if (reference_tile_) {
        friction_value = reference_tile_->friction_;
        if (reference_tile_->deadly_) {
            SetState(EntityState::Dying);
        }
    } else if (map->GetDefaultTile()) {
        friction_value = map->GetDefaultTile()->friction_;
    } else {
        friction_value = friction_;
    }

    float friction_x = (speed_.x * friction_value.x) * delta_time;
    float friction_y = (speed_.y * friction_value.y) * delta_time;
    ApplyFriction({ friction_x, friction_y });
    sf::Vector2f delta_position = velocity_ * delta_time;
    Move({ delta_position.x, delta_position.y });
    colliding_on_x_ = false;
    colliding_on_y_ = false;
    CheckCollisions();
    ResolveCollisions();
}

void BaseEntity::UpdateAABB()
{
    hit_box_ = sf::FloatRect(position_.x - (size_.x / 2), position_.y - size_.y, size_.x, size_.y);
}

void BaseEntity::CheckCollisions()
{
    Map* game_map = entity_manager_->GetContext()->game_map_;
    auto tile_size = game_map->GetTileSize();
    int from_x = static_cast<int>(floor(hit_box_.left / static_cast<float>(tile_size)));
    int to_x = static_cast<int>(floor((hit_box_.left + hit_box_.width) / static_cast<float>(tile_size)));
    int from_y = static_cast<int>(floor(hit_box_.top / static_cast<float>(tile_size)));
    int to_y = static_cast<int>(floor((hit_box_.top + hit_box_.height) / static_cast<float>(tile_size)));

    for (auto x = from_x; x <= to_x; ++x) {
        for (auto y = from_y; y <= to_y; ++y) {
            auto tile = game_map->GetTile(x, y);
            if (!tile) {
                continue;
            }
            sf::FloatRect tile_bounds(static_cast<float>(x * tile_size), static_cast<float>(y * tile_size), static_cast<float>(tile_size), static_cast<float>(tile_size));
            sf::FloatRect intersection;
            hit_box_.intersects(tile_bounds, intersection);
            float area = intersection.width * intersection.height;

            CollisionElement e(area, tile->properties_, tile_bounds);
            collisions_.emplace_back(e);
            if (tile->warp_ && type_ == EntityType::Player) {
                game_map->LoadNext();
            }
        }
    }
}

void BaseEntity::ResolveCollisions()
{
    if (!collisions_.empty()) {
        std::sort(collisions_.begin(), collisions_.end(), SortCollisions); // lambda this up
        Map* game_map = entity_manager_->GetContext()->game_map_;
        auto tileSize = game_map->GetTileSize();
        for (auto& itr : collisions_) {
            if (!hit_box_.intersects(itr.tile_bounds_)) {
                continue;
            }
            // Debug
            if (entity_manager_->GetContext()->debug_overlay_.Debug()) {
                sf::Vector2f temp_position(itr.tile_bounds_.left, itr.tile_bounds_.top);
                auto rect = std::make_unique<sf::RectangleShape>(sf::Vector2f(static_cast<float>(tileSize), static_cast<float>(tileSize)));
                rect->setPosition(temp_position);
                rect->setFillColor({ 255, 255, 0, 150 });
                entity_manager_->GetContext()->debug_overlay_.Add(std::move(rect));
            }
            // End debug.
            float x_diff = (hit_box_.left + (hit_box_.width / 2)) - (itr.tile_bounds_.left + (itr.tile_bounds_.width / 2));
            float y_diff = (hit_box_.top + (hit_box_.height / 2)) - (itr.tile_bounds_.top + (itr.tile_bounds_.height / 2));
            float resolve = 0;
            if (std::abs(x_diff) > std::abs(y_diff)) {
                if (x_diff > 0) {
                    resolve = (itr.tile_bounds_.left + tileSize) - hit_box_.left;
                } else {
                    resolve = -((hit_box_.left + hit_box_.width) - itr.tile_bounds_.left);
                }
                Move({ resolve, 0 });
                velocity_.x = 0;
                colliding_on_x_ = true;
            } else {
                if (y_diff > 0) {
                    resolve = (itr.tile_bounds_.top + tileSize) - hit_box_.top;
                } else {
                    resolve = -((hit_box_.top + hit_box_.height) - itr.tile_bounds_.top);
                }
                Move({ 0, resolve });
                if (resolve <= 0) {
                    if (velocity_.y > 0) {
                        velocity_.y = 0;
                    }
                }
                reference_tile_ = itr.tile_;
                colliding_on_y_ = true;
            }
        }
        collisions_.clear();
    }
    if (!colliding_on_y_) {
        reference_tile_ = nullptr;
    }
}
