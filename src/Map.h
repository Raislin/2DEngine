#ifndef MAP_H
#define MAP_H

#include "BaseState.h"
#include "SharedContext.h"
#include "Types.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>

enum Sheet : unsigned { Tile_Size = 32,
    Sheet_Width = 256,
    Sheet_Height = 256 };
using TileID = unsigned;

struct TileInfo {
    TileInfo() = delete;
    TileInfo(SharedContext* shared_context, const ResourceName& texture_name = "",
        TileID id = 0)
        : shared_context_(shared_context)
    {
        if (texture_name == "") {
            return;
        }

        auto texture_manager = shared_context->texture_manager_;
        texture_ = texture_manager->GetResource(texture_name);
        id_ = id;
        sprite_.setTexture(*texture_);
        sf::IntRect tile_boundaries(
            id_ % (Sheet::Sheet_Width / Sheet::Tile_Size) * Sheet::Tile_Size,
            id_ / (Sheet::Sheet_Height / Sheet::Tile_Size) * Sheet::Tile_Size,
            Sheet::Tile_Size, Sheet::Tile_Size);
        sprite_.setTextureRect(tile_boundaries);
    }

    std::shared_ptr<const sf::Texture> texture_;
    sf::Sprite sprite_;
    TileID id_ = 0;
    std::string name_;
    sf::Vector2f friction_;
    bool deadly_ = false;
    SharedContext* shared_context_;
};

struct Tile {
    TileInfo* properties_ = nullptr;
    bool warp_ = false;
};

using TileMap = std::unordered_map<TileID, std::unique_ptr<Tile>>;
using TileSet = std::unordered_map<TileID, std::unique_ptr<TileInfo>>;

class Map {
public:
    Map(SharedContext* context, BaseState* current_state);
    ~Map();
    Tile* GetTile(unsigned x, unsigned y);
    TileInfo* GetDefaultTile();
    float GetGravity() const;
    unsigned int GetTileSize() const;
    const sf::Vector2u& GetMapSize() const;
    const sf::Vector2f& GetPlayerStart() const;
    void LoadMap(const FilePath path);
    void LoadNext();
    void Update(float delta_time);
    void Draw();

private:
    unsigned ConvertCoords2Dto1D(unsigned x, unsigned y);
    void LoadTiles(const FilePath path);
    void PurgeMap();
    void PurgeTileSet();
    TileSet tile_set_;
    TileMap tile_map_;
    sf::Sprite background_;
    TileInfo default_tile_;
    sf::Vector2u max_map_size_ = { 32, 32 };
    sf::Vector2f player_start_ = { 0.0f, 0.0f };
    unsigned tile_count_ = 0;
    unsigned tile_set_count_ = 0;
    float map_gravity_ = 512.0f;
    std::string next_map_;
    bool load_next_map_ = false;
    std::shared_ptr<const sf::Texture> background_texture_;
    BaseState* current_state_ = nullptr;
    SharedContext* context_ = nullptr;
};

#endif // !MAP_H
