#pragma once

#include <vector>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "entities/Player.h"
#include "entities/Bullet.h"
#include "utils/Timer.h"


enum class GameState
{
	StartScreen,
	Gameplay,
};


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

	// Current game state
	GameState m_GameState = GameState::StartScreen;

	// Whether the states have been initialised yet
	bool m_StartScreenInitialised = false;
	bool m_GameplayInitialised = false;

private:
	// Handles user input for gameplay state
	void handleStartScreenEvents();
	// Updates the gameplay
	void updateStartScreen();
	// Renders gameplay to the screen
	void drawStartScreen();

	// Handles user input for gameplay state
	void handleGameplayEvents();
	// Updates the gameplay
	void updateGameplay();
	// Renders gameplay to the screen
	void drawGameplay();

	// Initialises the different states
	void initStartScreen();
	void initGameplay();

public:
	Game();
	~Game();

	// Runs the main-loop
	void run();
};
