#include "Window.h"
#include "Game.h"

int main() {
	Game game;
	Window window(&game, 1920, 1080, "Minecraft 2");
	window.WindowLoop();

	return 0;
}
