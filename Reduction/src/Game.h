#pragma once

#include <SDL/SDL.h>

#include "entities/Player.h"


class Game
{
private:
	// Set to false on an error, or on quit
	bool m_Running = true;

	// SDL members
	SDL_Window* m_Window = nullptr;
	SDL_Renderer* m_Renderer = nullptr;
	SDL_Event m_Event;

	// Players
	Player* m_Player = nullptr;

private:
	// Handles user input
	void handleEvents();
	// Updates the gameplay
	void update();
	// Renders to the screen
	void draw();

public:
	Game();
	~Game();

	// Runs the main-loop
	void run();
};
