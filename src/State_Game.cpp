#include "State_Game.h"
#include "StateManager.h"

State_Game::State_Game(StateManager* state_manager)
    : BaseState(state_manager)
{
}

State_Game::~State_Game() {}

void State_Game::OnCreate()
{
    state_manager_->GetContext()->event_manager_->AddCallback(
        StateType::kGame, "Key_Pause", &State_Game::Pause, this);
    state_manager_->GetContext()->event_manager_->AddCallback(
        StateType::kGame, "Key_Debug", &State_Game::ToggleOverlay, this);
    state_manager_->GetContext()->event_manager_->AddCallback(
        StateType::kGame, "Key_Escape", &State_Game::MainMenu, this);

    sf::Vector2u size = state_manager_->GetContext()->window_->GetWindowSize();
    view_.setSize(sf::Vector2f(size));
    view_.setCenter(size.x / 2.0f, size.y / 2.0f);
    view_.zoom(0.6f);
    state_manager_->GetContext()->window_->GetRenderWindow()->setView(view_);

    game_map_ = std::make_unique<Map>(state_manager_->GetContext(), this);
    game_map_->LoadMap("resources/media/maps/map1.map");
}

void State_Game::OnDestroy()
{
    state_manager_->GetContext()->event_manager_->RemoveCallback(StateType::kGame,
        "Key_Pause");
    state_manager_->GetContext()->event_manager_->RemoveCallback(StateType::kGame,
        "Key_Escape");
    state_manager_->GetContext()->event_manager_->RemoveCallback(StateType::kGame,
        "Key_Debug");
}

void State_Game::Update(const sf::Time& time)
{
    SharedContext* context = state_manager_->GetContext();
    BaseEntity* player = context->entity_manager_->Find("Player");
    if (!player) {
        std::cout << "Respawning player..." << '\n';
        context->entity_manager_->Add(EntityType::Player, "Player");
        player = context->entity_manager_->Find("Player");
        player->SetPosition(game_map_->GetPlayerStart());
    } else {
        view_.setCenter(player->GetPosition());
        context->window_->GetRenderWindow()->setView(view_);
    }

    sf::FloatRect view_space = context->window_->GetViewSpace();
    if (view_space.left <= 0) {
        view_.setCenter(view_space.width / 2, view_.getCenter().y);
        context->window_->GetRenderWindow()->setView(view_);
    } else if (view_space.left + view_space.width > (game_map_->GetMapSize().x + 1) * Sheet::Tile_Size) {
        view_.setCenter(((game_map_->GetMapSize().x + 1) * Sheet::Tile_Size) - (view_space.width / 2), view_.getCenter().y);
        context->window_->GetRenderWindow()->setView(view_);
    }

    game_map_->Update(time.asSeconds());
    state_manager_->GetContext()->entity_manager_->Update(time.asSeconds());
}

void State_Game::Draw() const
{
    game_map_->Draw();
    state_manager_->GetContext()->entity_manager_->Draw();
}

void State_Game::MainMenu(const EventDetails* const)
{
    state_manager_->SwitchTo(StateType::kMainMenu);
}

void State_Game::Pause(const EventDetails* const)
{
    state_manager_->SwitchTo(StateType::kPaused);
}

void State_Game::Activate() {}
void State_Game::Deactivate() {}

// Test/debug methods.
void State_Game::ToggleOverlay(const EventDetails* const)
{
    state_manager_->GetContext()->debug_overlay_.SetDebug(!state_manager_->GetContext()->debug_overlay_.Debug());
}
