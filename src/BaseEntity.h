#ifndef BASE_ENTITY_H
#define BASE_ENTITY_H

#include "Types.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

enum class EntityType { Base,
    Enemy,
    Player };

enum class EntityState { Idle,
    Walking,
    Jumping,
    Attacking,
    Hurt,
    Dying };

struct TileInfo;

struct CollisionElement {
    CollisionElement(float area, TileInfo* info, const sf::FloatRect& bounds)
        : area_(area)
        , tile_(info)
        , tile_bounds_(bounds)
    {
    }
    float area_;
    TileInfo* tile_;
    sf::FloatRect tile_bounds_;
};

using Collisions = std::vector<CollisionElement>;
bool SortCollisions(const CollisionElement& one, const CollisionElement& two);

class EntityManager;
class BaseEntity {
    friend class EntityManager;

public:
    BaseEntity(EntityManager* entity_manager);
    virtual ~BaseEntity();

    const sf::Vector2f& GetPosition() const;
    const sf::Vector2f& GetSize() const;
    EntityState GetState() const;
    std::string GetName() const;
    EntityID GetId() const;
    EntityType GetType() const;

    void SetPosition(const sf::Vector2f position);
    void SetSize(const sf::Vector2f size);
    void SetState(const EntityState& state);

    void Move(const sf::Vector2f distance);
    void AddVelocity(const sf::Vector2f velocity);
    void Accelerate(sf::Vector2f acceleration);
    void SetAcceleration(const sf::Vector2f acceleration);
    void ApplyFriction(const sf::Vector2f friction);

    virtual void Update(float delta_time);
    virtual void Draw(sf::RenderWindow* window) = 0;

protected:
    void UpdateAABB();
    void CheckCollisions();
    void ResolveCollisions();

    virtual void OnEntityCollision(BaseEntity* collider, bool attack) = 0;

    std::string name_ = "BaseEntity";
    EntityType type_ = EntityType::Base;
    EntityID id_;
    sf::Vector2f position_;
    sf::Vector2f old_position_;
    sf::Vector2f velocity_;
    sf::Vector2f max_velocity_;
    sf::Vector2f speed_;
    sf::Vector2f acceleration_;
    sf::Vector2f friction_ = { 0.8f, 0.0f };
    TileInfo* reference_tile_ = nullptr;
    sf::Vector2f size_;
    sf::FloatRect hit_box_;
    EntityState state_ = EntityState::Idle;

    bool colliding_on_x_ = false;
    bool colliding_on_y_ = false;

    Collisions collisions_;
    EntityManager* entity_manager_;
};

#endif
