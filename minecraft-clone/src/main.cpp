#include <iostream>
#include "Window.h"
#include "Game.h"

int main() {
	glfw::Window window(1920, 1080, "Mineclone"); // initialize window

	Game game; // initialize game
	
	window.WindowLoop(game);
	
	return 0;
}
