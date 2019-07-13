#ifndef BASESTATE_H
#define BASESTATE_H

#include <SFML/Graphics/View.hpp>
#include <SFML/System.hpp>

enum class StateType {
    kIntro = 1,
    kMainMenu,
    kGame,
    kPaused,
    kGameOver,
    kCredits,
};

class StateManager;

class BaseState {
    friend class StateManager;

public:
    BaseState(StateManager* state_manager)
        : state_manager_(state_manager)
        , is_transparent_(false)
        , is_transcendent_(false)
    {
    }
    virtual ~BaseState() = default;

    virtual void OnCreate() = 0;
    virtual void OnDestroy() = 0;

    virtual void Activate() = 0;
    virtual void Deactivate() = 0;

    virtual void Update(const sf::Time& time) = 0;
    virtual void Draw() const = 0;

    void SetTransparent(const bool& transparent)
    {
        is_transparent_ = transparent;
    }
    bool IsTransparent() const { return is_transparent_; }
    void SetTranscendent(const bool& transcendent)
    {
        is_transcendent_ = transcendent;
    }
    bool IsTranscendent() const { return is_transcendent_; }

    StateManager* GetStateManager() const { return state_manager_; }
    const sf::View& GetView() const { return view_; }
    void SetView(const sf::View& view) { view_ = view; };

protected:
    StateManager* state_manager_;
    sf::View view_;
    bool is_transparent_;
    bool is_transcendent_;
};

#endif // BASESTATE_H
