#ifndef CHARACTER_H
#define CHARACTER_H

#include "BaseEntity.h"
#include "SpriteSheet.h"

class Character : public BaseEntity {
    friend class EntityManager;

public:
    Character(EntityManager* entity_manager);
    virtual ~Character();

    void Move(const Direction& direction);

    void Jump();
    void Attack();
    void GetHurt(const int& damage);

    void Load(const FilePath path);

    virtual void OnEntityCollision(
        BaseEntity* collider, bool attack)
        = 0;

    virtual void Update(float delta_time);
    void Draw(sf::RenderWindow* window);

protected:
    void UpdateAttackHitBox();
    void Animate();
    SpriteSheet sprite_sheet_;
    float jump_velocity_ = 250;

    int hit_points_ = 5;

    sf::FloatRect attack_hit_box_;
    sf::Vector2f attack_hit_box_offset_;
};

#endif // !CHARACTER_H
