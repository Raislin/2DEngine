#include "Map.h"
#include "StateManager.h"
#include "Utilities.h"
#include <cmath>

Map::Map(SharedContext* context, BaseState* current_state)
    : context_(context)
    , default_tile_(context)
    , current_state_(current_state)
{
    context->game_map_ = this;
    LoadTiles("resources/tiles.cfg");
}

Map::~Map()
{
    context_->game_map_ = nullptr;
}

Tile* Map::GetTile(unsigned x, unsigned y)
{
    auto itr = tile_map_.find(ConvertCoords2Dto1D(x, y));
    return (itr != tile_map_.end() ? itr->second.get() : nullptr);
}

TileInfo* Map::GetDefaultTile() { return &default_tile_; }

float Map::GetGravity() const { return map_gravity_; }

unsigned int Map::GetTileSize() const { return Sheet::Tile_Size; }

const sf::Vector2u& Map::GetMapSize() const { return max_map_size_; }

const sf::Vector2f& Map::GetPlayerStart() const { return player_start_; }

void Map::LoadMap(const FilePath path)
{
    std::ifstream map_file;
    map_file.open(Utils::GetExecutableDirectory() + path);
    if (!map_file.is_open()) {
        std::cerr << "! Failed loading map file: " << path << '\n';
        return;
    }
    auto entity_manager = context_->entity_manager_;
    std::string line;
    std::cout << "--- Loading a map: " << path << '\n';

    EntityID player_id;
    while (std::getline(map_file, line)) {
        if (line[0] == '|') {
            continue;
        }
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "TILE") {
            int tile_id = 0;
            keystream >> tile_id;
            if (tile_id < 0) {
                std::cerr << "! Bad tile id: " << tile_id << std::endl;
                continue;
            }
            auto itr = tile_set_.find(tile_id);
            if (itr == tile_set_.end()) {
                std::cerr << "! Tile id(" << tile_id << ") was not found in tileset." << std::endl;
                continue;
            }
            sf::Vector2u tile_coordinates;
            keystream >> tile_coordinates.x >> tile_coordinates.y;
            if (tile_coordinates.x > max_map_size_.x || tile_coordinates.y > max_map_size_.y) {
                std::cerr << "! Tile is out of range: " << tile_coordinates.x << " " << tile_coordinates.y << std::endl;
                continue;
            }
            auto tile = std::make_unique<Tile>();
            // Bind properties of a tile from a set.
            tile->properties_ = itr->second.get();
            std::string warp;
            keystream >> warp;
            if (warp == "WARP") {
                tile->warp_ = true;
            }
            if (!tile_map_.emplace(ConvertCoords2Dto1D(tile_coordinates.x, tile_coordinates.y), std::move(tile)).second) {
                // Duplicate tile detected!
                std::cerr << "! Duplicate tile! : " << tile_coordinates.x
                          << "" << tile_coordinates.y << std::endl;
                tile.reset();
                continue;
            }
        } else if (type == "BACKGROUND") {
            ResourceName background_texture;
            keystream >> background_texture;
            background_texture_ = context_->texture_manager_->GetResource(background_texture);
            if (!background_texture_) {
                continue;
            }
            background_.setTexture(*background_texture_);
            sf::Vector2f viewSize = current_state_->GetView().getSize();
            sf::Vector2u textureSize = background_texture_->getSize();
            sf::Vector2f scale_factors;
            scale_factors.x = viewSize.x / textureSize.x;
            scale_factors.y = viewSize.y / textureSize.y;
            background_.setScale(scale_factors);
        } else if (type == "SIZE") {
            keystream >> max_map_size_.x >> max_map_size_.y;
        } else if (type == "GRAVITY") {
            keystream >> map_gravity_;
        } else if (type == "DEFAULT_FRICTION") {
            keystream >> default_tile_.friction_.x >> default_tile_.friction_.y;
        } else if (type == "NEXTMAP") {
            keystream >> next_map_;
        } else if (type == "PLAYER") {
            if (player_id) {
                continue;
            }
            // Set up the player position here.
            player_id = entity_manager->Add(EntityType::Player);
            if (!player_id) {
                continue;
            }
            float player_x = 0;
            float player_y = 0;
            keystream >> player_x >> player_y;
            entity_manager->Find(player_id)->SetPosition({ player_x, player_y });
            player_start_ = sf::Vector2f(player_x, player_y);
        } else if (type == "ENEMY") {
            std::string enemyName;
            keystream >> enemyName;
            EntityID enemy_id = entity_manager->Add(EntityType::Enemy, enemyName);
            if (!enemy_id) {
                continue;
            }
            float enemy_x = 0;
            float enemy_y = 0;
            keystream >> enemy_x >> enemy_y;
            entity_manager->Find(enemy_id)->SetPosition({ enemy_x, enemy_y });
        } else {
            // Something else.
            std::cerr << "! Unknown type \"" << type << "\"." << std::endl;
        }
    }
    map_file.close();
    std::cout << "--- Map Loaded! ---" << std::endl;
}

void Map::LoadNext() { load_next_map_ = true; }

void Map::Update(float)
{
    if (load_next_map_) {
        PurgeMap();
        load_next_map_ = false;
        if (next_map_ != "") {
            LoadMap("resources/media/maps/" + next_map_);
        } else {
            current_state_->GetStateManager()->SwitchTo(StateType::kGameOver);
        }
        next_map_ = "";
    }
    sf::FloatRect view_space = context_->window_->GetViewSpace();
    background_.setPosition(view_space.left, view_space.top);
}

void Map::Draw()
{
    auto window_ = context_->window_->GetRenderWindow();
    window_->draw(background_);
    auto view_space = context_->window_->GetViewSpace();
    sf::Vector2i tile_begin(
        static_cast<int>(std::floor(view_space.left / static_cast<float>(Sheet::Tile_Size))),
        static_cast<int>(std::floor(view_space.top / static_cast<float>(Sheet::Tile_Size))));
    sf::Vector2i tile_end(
        static_cast<int>(std::ceil((view_space.left + view_space.width) / static_cast<float>(Sheet::Tile_Size))),
        static_cast<int>(std::ceil((view_space.top + view_space.height) / static_cast<float>(Sheet::Tile_Size))));

    for (int x = tile_begin.x; x <= tile_end.x; ++x) {
        for (int y = tile_begin.y; y <= tile_end.y; ++y) {
            if (x < 0 || y < 0) {
                continue;
            }
            auto tile = GetTile(x, y);
            if (!tile) {
                continue;
            }
            sf::Sprite& sprite = tile->properties_->sprite_;
            sprite.setPosition(x * static_cast<float>(Sheet::Tile_Size),
                y * static_cast<float>(Sheet::Tile_Size));
            window_->draw(sprite);
        }
    }
}

unsigned int Map::ConvertCoords2Dto1D(unsigned x, unsigned y)
{
    return (x * max_map_size_.x) + y;
}

void Map::LoadTiles(const FilePath path)
{
    std::ifstream file;
    file.open(Utils::GetExecutableDirectory() + path);
    if (!file.is_open()) {
        std::cout << "! Failed loading tile set file: "
                  << path << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') { // Ignore comment
            continue;
        }

        std::stringstream keystream(line);
        int tile_id;
        keystream >> tile_id;

        if (tile_id < 0) {
            continue;
        }

        auto tile = std::make_unique<TileInfo>(context_, "tilesheet", tile_id);
        keystream >> tile->name_ >> tile->friction_.x
            >> tile->friction_.y >> tile->deadly_;

        if (!tile_set_.emplace(tile_id, std::move(tile)).second) {
            std::cout << "! Duplicate tile type: "
                      << tile->name_ << std::endl;
        }
    }
    file.close();
}

void Map::PurgeMap()
{
    tile_count_ = 0;
    tile_map_.clear();
    context_->entity_manager_->Purge();
    background_texture_.reset();
}

void Map::PurgeTileSet()
{
    tile_set_.clear();
    tile_set_count_ = 0;
}
