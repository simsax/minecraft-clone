#include <iostream>
#include "Window.h"
#include "Game.h"
#include "Chunk.h"

int main() {
	glfw::Window window(1920, 1080, "Minecraft 2"); // initialize window

	Game game; // initialize game

	window.WindowLoop(game);

	return 0;
}
