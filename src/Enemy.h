#ifndef ENEMY_H
#define ENEMY_H

#include "Character.h"

class Enemy : public Character {
public:
    Enemy(EntityManager* entity_manager);
    ~Enemy();

    void OnEntityCollision(BaseEntity* collider, bool attack);
    void Update(float delta_time);

private:
    sf::Vector2f destination_;
    bool has_destination_ = false;
};

#endif // !ENEMY_H
