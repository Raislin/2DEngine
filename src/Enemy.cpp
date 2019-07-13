#include "Enemy.h"

Enemy::Enemy(EntityManager* entity_manager)
    : Character(entity_manager)
{
    type_ = EntityType::Enemy;
}

Enemy::~Enemy() {}

void Enemy::OnEntityCollision(BaseEntity* collider, bool attack)
{
    if (state_ == EntityState::Dying) {
        return;
    }
    if (attack) {
        return;
    }
    if (collider->GetType() != EntityType::Player) {
        return;
    }
    auto player = static_cast<Character*>(collider);
    SetState(EntityState::Attacking);
    player->GetHurt(1);
    if (position_.x > player->GetPosition().x) {
        player->AddVelocity({ -speed_.x, 0 });
        sprite_sheet_.SetDirection(Direction::kLeft);
    } else {
        player->AddVelocity({ speed_.y, 0 });
        sprite_sheet_.SetDirection(Direction::kRight);
    }
}

void Enemy::Update(float delta_time)
{
    Character::Update(delta_time);

    if (has_destination_) {
        if (std::abs(destination_.x - position_.x) < 16) {
            has_destination_ = false;
            return;
        }
        if (destination_.x - position_.x > 0) {
            Move(Direction::kRight);
        } else {
            Move(Direction::kLeft);
        }
        if (colliding_on_x_) {
            has_destination_ = false;
        }
        return;
    }
    int random = rand() % 1000 + 1;
    if (random != 1000) {
        return;
    }
    int new_x = rand() % 65 + 0;
    if (rand() % 2) {
        new_x = -new_x;
    }
    destination_.x = position_.x + new_x;
    if (destination_.x < 0) {
        destination_.x = 0;
    }
    has_destination_ = true;
}
