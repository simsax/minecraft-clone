#include "graphics/Window.h"
#include "Game.h"

int main()
{
    Game game;
    Window window(&game, game.GetWidth(), game.GetHeight(), "Minecraft clone");
    window.WindowLoop();

    return 0;
}
