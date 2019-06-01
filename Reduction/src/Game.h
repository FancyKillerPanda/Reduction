#pragma once

#include <vector>

#include <SDL/SDL.h>

#include "entities/Player.h"
#include "entities/Bullet.h"
#include "utils/Timer.h"

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
	std::vector<Player*> m_Players;

	// FPS clock
	Timer m_FrameTimer;

	// Wall
	double m_WallScale = 1;
	SDL_Texture* m_WallTexture;
	SDL_Rect m_WallRect;
	unsigned int m_OriginalWallWidth;
	unsigned int m_OriginalWallHeight;

	// Background
	SDL_Texture* m_SpaceBackgroundTexture = nullptr;
	SDL_Rect m_SpaceBackgroundRect;

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
