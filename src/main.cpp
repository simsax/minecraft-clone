#include "Game.h"

int main()
{
    Game* game = new Game();
    game->Run();
    delete game;

    return 0;
}
