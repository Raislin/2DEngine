#ifndef SHARED_CONTEXT_H
#define SHARED_CONTEXT_H

#include "DebugOverlay.h"
#include "EntityManager.h"
#include "EventManager.h"
#include "TextureManager.h"
#include "Window.h"

class Map;

struct SharedContext {
    SharedContext() {}
    Window* window_ = nullptr;
    EventManager* event_manager_ = nullptr;
    TextureManager* texture_manager_ = nullptr;
    EntityManager* entity_manager_ = nullptr;
    DebugOverlay debug_overlay_;
    Map* game_map_ = nullptr;
};

#endif
