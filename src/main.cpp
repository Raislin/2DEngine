#include "Game.h"

int main(int, char **[]) {
  // Program entry point.
  Game game; // Creating our game object.
  while (!game.GetWindow()->IsDone()) {
    // Game loop.
    // game.HandleInput();
    game.Update();
    game.Render();
    game.LateUpdate();
  }

  return 0;
}
