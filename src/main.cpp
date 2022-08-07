#include "graphics/Window.h"
#include "Game.h"

int main()
{
    Game* game = new Game;
    Window window(game, 1920, 1080, "Minecraft 2");
    window.WindowLoop();
    delete game;

    return 0;
}
