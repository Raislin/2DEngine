#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "BaseEntity.h"
#include "Types.h"
#include <functional>
#include <memory>
#include <unordered_map>

using EntityContainer = std::unordered_map<EntityID, std::unique_ptr<BaseEntity>>;
using EntityFactory = std::unordered_map<EntityType, std::function<std::unique_ptr<BaseEntity>()>>;
using EnemyTypes = std::unordered_map<std::string, std::string>;

struct SharedContext;

class EntityManager {
public:
    EntityManager(SharedContext* context, unsigned max_entities);
    ~EntityManager();

    EntityID Add(const EntityType& type, const std::string& name = "");
    BaseEntity* Find(EntityID id);
    BaseEntity* Find(const std::string& name);
    void Remove(EntityID id);

    void Update(float delta_time);
    void Draw();

    void Purge();

    SharedContext* GetContext();

private:
    template <class T>
    void RegisterEntity(const EntityType& type)
    {
        entity_factory_[type] = [this]() -> std::unique_ptr<BaseEntity> {
            return std::make_unique<T>(this);
        };
    }

    void ProcessRemovals();
    void LoadEnemyTypes(const FilePath name);
    void EntityCollisionCheck();

    EntityContainer entities_;
    EnemyTypes enemy_types_;
    EntityFactory entity_factory_;
    SharedContext* context_;
    EntityID id_counter_ = 0;
    unsigned max_entities_;

    std::vector<EntityID> entities_to_remove_;
};

#endif // !ENTITY_MANAGER_H
