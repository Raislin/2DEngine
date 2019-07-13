#ifndef PLAYER_H
#define PLAYER_H

#include "Character.h"
#include "EventManager.h"

class Player : public Character {
public:
    Player(EntityManager* entity_manager);
    ~Player();

    void OnEntityCollision(BaseEntity* collider, bool attack);
    void React(const EventDetails* const details);
};

#endif // !PLAYER_H
