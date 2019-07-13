#include "Character.h"
#include "EntityManager.h"
#include "StateManager.h"

Character::Character(EntityManager* entity_manager)
    : BaseEntity(entity_manager)
    , sprite_sheet_(entity_manager_->GetContext()->texture_manager_)
{
    name_ = "Character";
}

Character::~Character() {}

void Character::Move(const Direction& direction)
{
    if (GetState() == EntityState::Dying) {
        return;
    }
    sprite_sheet_.SetDirection(direction);
    if (direction == Direction::kLeft) {
        Accelerate({ -speed_.x, 0 });
    } else {
        Accelerate({ speed_.x, 0 });
    }
    if (GetState() == EntityState::Idle) {
        SetState(EntityState::Walking);
    }
}

void Character::Jump()
{
    if (GetState() == EntityState::Dying || GetState() == EntityState::Jumping || GetState() == EntityState::Hurt) {
        return;
    }
    SetState(EntityState::Jumping);
    AddVelocity({ 0, -jump_velocity_ });
}

void Character::Attack()
{
    if (GetState() == EntityState::Dying || GetState() == EntityState::Jumping || GetState() == EntityState::Hurt || GetState() == EntityState::Attacking) {
        return;
    }
    SetState(EntityState::Attacking);
}

void Character::GetHurt(const int& damage)
{
    if (GetState() == EntityState::Dying || GetState() == EntityState::Hurt) {
        return;
    }
    hit_points_ = (hit_points_ - damage > 0 ? hit_points_ - damage : 0);
    if (hit_points_) {
        SetState(EntityState::Hurt);
    } else {
        SetState(EntityState::Dying);
    }
}

void Character::Load(const FilePath path)
{
    std::ifstream file;
    file.open(Utils::GetExecutableDirectory() + std::string("resources/media/characters/") + path);
    if (!file.is_open()) {
        std::cerr << "! Failed loading the character file: " << path << '\n';
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') {
            continue;
        }
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "Name") {
            keystream >> name_;
        } else if (type == "Spritesheet") {
            std::string file_path;
            keystream >> file_path;
            sprite_sheet_.LoadSheet("resources/media/spritesheets/" + file_path);
        } else if (type == "Hitpoints") {
            keystream >> hit_points_;
        } else if (type == "BoundingBox") {
            sf::Vector2f bounding_size;
            keystream >> bounding_size.x >> bounding_size.y;
            SetSize({ bounding_size.x, bounding_size.y });
        } else if (type == "DamageBox") {
            keystream >> attack_hit_box_offset_.x >> attack_hit_box_offset_.y
                >> attack_hit_box_.width >> attack_hit_box_.height;
        } else if (type == "Speed") {
            keystream >> speed_.x >> speed_.y;
        } else if (type == "JumpVelocity") {
            keystream >> jump_velocity_;
        } else if (type == "MaxVelocity") {
            keystream >> max_velocity_.x >> max_velocity_.y;
        } else {
            std::cout << "! Unknown type in character file: " << type << std::endl;
        }
    }
    file.close();
}

void Character::UpdateAttackHitBox()
{
    attack_hit_box_.left = (sprite_sheet_.GetDirection() == Direction::kLeft ? (hit_box_.left - attack_hit_box_.width) - attack_hit_box_offset_.x
                                                                             : (hit_box_.left + hit_box_.width) + attack_hit_box_offset_.x);
    attack_hit_box_.top = hit_box_.top + attack_hit_box_offset_.y;
}

void Character::Animate()
{
    EntityState state = GetState();

    if (state == EntityState::Walking && sprite_sheet_.GetCurrentAnimation()->GetName() != "Walk") {
        sprite_sheet_.SetAnimation("Walk", true, true);
    } else if (state == EntityState::Jumping && sprite_sheet_.GetCurrentAnimation()->GetName() != "Jump") {
        sprite_sheet_.SetAnimation("Jump", true, false);
    } else if (state == EntityState::Attacking && sprite_sheet_.GetCurrentAnimation()->GetName() != "Attack") {
        sprite_sheet_.SetAnimation("Attack", true, false);
    } else if (state == EntityState::Hurt && sprite_sheet_.GetCurrentAnimation()->GetName() != "Hurt") {
        sprite_sheet_.SetAnimation("Hurt", true, false);
    } else if (state == EntityState::Dying && sprite_sheet_.GetCurrentAnimation()->GetName() != "Death") {
        sprite_sheet_.SetAnimation("Death", true, false);
    } else if (state == EntityState::Idle && sprite_sheet_.GetCurrentAnimation()->GetName() != "Idle") {
        sprite_sheet_.SetAnimation("Idle", true, true);
    }
}

void Character::Update(float delta_time)
{
    BaseEntity::Update(delta_time);
    if (attack_hit_box_.width != 0 && attack_hit_box_.height != 0) {
        UpdateAttackHitBox();

        // Debug.
        if (entity_manager_->GetContext()->debug_overlay_.Debug()) {
            auto attack_rect = std::make_unique<sf::RectangleShape>(sf::Vector2f(attack_hit_box_.width, attack_hit_box_.height));
            attack_rect->setPosition(attack_hit_box_.left, attack_hit_box_.top);
            attack_rect->setFillColor({ static_cast<sf::Uint8>(255),
                static_cast<sf::Uint8>(0),
                static_cast<sf::Uint8>(0),
                (state_ == EntityState::Attacking && sprite_sheet_.GetCurrentAnimation()->IsInAction()
                        ? static_cast<sf::Uint8>(200)
                        : static_cast<sf::Uint8>(100)) });
            entity_manager_->GetContext()->debug_overlay_.Add(std::move(attack_rect));
        }
        // End debug.
    }
    if (GetState() != EntityState::Dying && GetState() != EntityState::Attacking && GetState() != EntityState::Hurt) {
        if (std::abs(velocity_.y) >= 0.001f) {
            SetState(EntityState::Jumping);
        } else if (std::abs(velocity_.x) >= 0.1f) {
            SetState(EntityState::Walking);
        } else {
            SetState(EntityState::Idle);
        }
    } else if (GetState() == EntityState::Attacking || GetState() == EntityState::Hurt) {
        if (!sprite_sheet_.GetCurrentAnimation()->IsPlaying()) {
            SetState(EntityState::Idle);
        }
    } else if (GetState() == EntityState::Dying) {
        if (!sprite_sheet_.GetCurrentAnimation()->IsPlaying()) {
            entity_manager_->Remove(id_);
        }
    }

    Animate();

    sprite_sheet_.Update(delta_time);
    sprite_sheet_.SetSpritePosition(position_);
}

void Character::Draw(sf::RenderWindow* window)
{
    sprite_sheet_.Draw(window);
}
