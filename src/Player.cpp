#include "Player.h"
#include "EntityManager.h"
#include "StateManager.h"

Player::Player(EntityManager* entity_manager)
    : Character(entity_manager)
{
    Load("player.char");
    type_ = EntityType::Player;

    EventManager* events = entity_manager->GetContext()->event_manager_;
    events->AddCallback<Player>(StateType::kGame, "Player_MoveLeft", &Player::React, this);
    events->AddCallback<Player>(StateType::kGame, "Player_MoveRight", &Player::React, this);
    events->AddCallback<Player>(StateType::kGame, "Player_Jump", &Player::React, this);
    events->AddCallback<Player>(StateType::kGame, "Player_Attack", &Player::React, this);
}

Player::~Player()
{
    EventManager* events = entity_manager_->GetContext()->event_manager_;
    events->RemoveCallback(StateType::kGame, "Player_MoveLeft");
    events->RemoveCallback(StateType::kGame, "Player_MoveRight");
    events->RemoveCallback(StateType::kGame, "Player_Jump");
    events->RemoveCallback(StateType::kGame, "Player_Attack");
}

void Player::React(const EventDetails* const details)
{
    if (details->name_ == "Player_MoveLeft") {
        Character::Move(Direction::kLeft);
    } else if (details->name_ == "Player_MoveRight") {
        Character::Move(Direction::kRight);
    } else if (details->name_ == "Player_Jump") {
        std::cerr << "Jumping...\n";
        Character::Jump();
    } else if (details->name_ == "Player_Attack") {
        Character::Attack();
    }
}

void Player::OnEntityCollision(BaseEntity* collider, bool attack)
{
    if (state_ == EntityState::Dying) {
        return;
    }
    if (attack) {
        if (state_ != EntityState::Attacking) {
            return;
        }
        if (!sprite_sheet_.GetCurrentAnimation()->IsInAction()) {
            return;
        }
        if (collider->GetType() != EntityType::Enemy && collider->GetType() != EntityType::Player) {
            return;
        }
        auto opponent = static_cast<Character*>(collider);
        opponent->GetHurt(1);
        if (position_.x > opponent->GetPosition().x) {
            opponent->AddVelocity({ -32, 0 });
        } else {
            opponent->AddVelocity({ 32, 0 });
        }
    } else {
        // Other behavior.
    }
}
