#include "EntityManager.h"
#include "Enemy.h"
#include "Player.h"
#include "SharedContext.h"
#include "Utilities.h"
#include <functional>

namespace {
const auto& get_name = Utils::get_second;
}

EntityManager::EntityManager(SharedContext* context, unsigned max_entities)
    : context_(context)
    , max_entities_(max_entities)
{
    LoadEnemyTypes("enemy_list.list");
    RegisterEntity<Player>(EntityType::Player);
    RegisterEntity<Enemy>(EntityType::Enemy);
}
EntityManager::~EntityManager() {}

EntityID EntityManager::Add(const EntityType& type, const std::string& name)
{

    auto factory_itr = entity_factory_.find(type);

    if (factory_itr == entity_factory_.end()) {
        return std::optional<unsigned>(std::nullopt);
    }

    auto entity = factory_itr->second();
    entity->id_ = id_counter_;

    if (name != "") {
        entity->name_ = name;
    }

    if (type == EntityType::Enemy) {
        auto enemy_itr = enemy_types_.find(name);
        if (enemy_itr != enemy_types_.end()) {
            auto enemy = static_cast<Enemy*>(entity.get());
            enemy->Load(get_name(*enemy_itr));
        }
    }

    entities_.emplace(id_counter_, std::move(entity));

    ++*id_counter_;
    return *id_counter_ - 1;
}

BaseEntity* EntityManager::Find(const std::string& name)
{
    auto entity = std::find_if(entities_.begin(), entities_.end(),
        [&](auto& i) { return i.second->GetName() == name; });
    return entity != std::end(entities_) ? entity->second.get() : nullptr;
}

BaseEntity* EntityManager::Find(EntityID id)
{
    auto itr = entities_.find(id);

    return itr != std::end(entities_) ? itr->second.get() : nullptr;
}

void EntityManager::Remove(EntityID id)
{
    entities_to_remove_.emplace_back(id);
}

void EntityManager::Update(float delta_time)
{
    if (entities_.empty()) {
        return;
    }

    for (const auto& i : entities_) {
        i.second->Update(delta_time);
    }

    EntityCollisionCheck();
    ProcessRemovals();
}

void EntityManager::Draw()
{
    sf::RenderWindow* window = context_->window_->GetRenderWindow();
    const auto view_space = context_->window_->GetViewSpace();

    for (const auto& i : entities_) {
        if (view_space.intersects(i.second->hit_box_)) {
            i.second->Draw(window);
        }
    }
}

void EntityManager::Purge()
{
    entities_.clear();
    id_counter_ = 0;
}

void EntityManager::ProcessRemovals()
{
    for (const auto& i : entities_to_remove_) {
        auto entity = entities_.find(i);
        std::cout << "Discarding entity: " << *(entity->second->GetId()) << '\n';
        entities_.erase(i);
    }
    entities_to_remove_.clear();
}

void EntityManager::EntityCollisionCheck()
{
    if (entities_.empty()) {
        return;
    }

    for (auto entity_1 = std::begin(entities_); std::next(entity_1) != std::end(entities_); ++entity_1) {
        for (auto entity_2 = std::next(entity_1); entity_2 != std::end(entities_); ++entity_2) {
            auto e1 = entity_1->second.get();
            auto e2 = entity_2->second.get();

            // Regular AABB bounding box collision.
            if (e1->hit_box_.intersects(e2->hit_box_)) {
                e1->OnEntityCollision(e2, false);
                e2->OnEntityCollision(e1, false);
            }

            auto t1 = e1->GetType();
            auto t2 = e2->GetType();
            if (t1 == EntityType::Player || t1 == EntityType::Enemy) {
                auto c1 = static_cast<Character*>(e1);
                if (c1->attack_hit_box_.intersects(e2->hit_box_)) {
                    c1->OnEntityCollision(e2, true);
                }
            }

            if (t2 == EntityType::Player || t2 == EntityType::Enemy) {
                auto c2 = static_cast<Character*>(e2);
                if (c2->attack_hit_box_.intersects(e1->hit_box_)) {
                    c2->OnEntityCollision(e1, true);
                }
            }
        }
    }
}
void EntityManager::LoadEnemyTypes(const FilePath name)
{
    std::ifstream file;
    file.open(Utils::GetExecutableDirectory() + std::string("resources/media/characters/") + name);
    if (!file.is_open()) {
        std::cerr << "! Failed loading file: " << name << '\n';
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') {
            continue;
        }
        std::stringstream keystream(line);
        std::string enemy_name;
        FilePath character_file;
        keystream >> enemy_name >> character_file;
        enemy_types_.emplace(enemy_name, character_file);
    }
    file.close();
}

SharedContext* EntityManager::GetContext() { return context_; }
