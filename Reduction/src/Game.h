#pragma once

#include <vector>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "entities/Player.h"
#include "entities/Bullet.h"
#include "utils/Timer.h"
#include "gfx/Text.h"
#include "gfx/Button.h"


enum class GameState
{
	StartScreen,
	Gameplay,
	RoundOver,
};

enum class StartScreenPage
{
	NumberOfPlayersChoice,
	RedPowerUp,
	BluePowerUp,
	GreyPowerUp,
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

	// "Reduction" header text
	Text m_ReductionText;

	// "Next" button
	Button* m_NextButton;

	// Number of players choice
	int m_NumberOfPlayers;
	Button* m_TwoPlayersButton;
	Button* m_ThreePlayersButton;

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

	// Current start screen page
	StartScreenPage m_StartScreenPage = StartScreenPage::NumberOfPlayersChoice;

	// Whether the states have been initialised yet
	bool m_StartScreenInitialised = false;
	bool m_GameplayInitialised = false;

	// Powerups
	SDL_Texture* m_SpeedPowerupTexture;
	SDL_Rect m_SpeedPowerupRect;
	Button* m_SpeedPowerupButton;
	SDL_Texture* m_AccuracyPowerupTexture;
	SDL_Rect m_AccuracyPowerupRect;
	Button* m_AccuracyPowerupButton;
	SDL_Texture* m_DamagePowerupTexture;
	SDL_Rect m_DamagePowerupRect;
	Button* m_DamagePowerupButton;
	SDL_Texture* m_CooldownPowerupTexture;
	SDL_Rect m_CooldownPowerupRect;
	Button* m_CooldownPowerupButton;

	// Text asking for powerups
	Text m_RedPowerupsText;
	Text m_BluePowerupsText;
	Text m_GreyPowerupsText;

	// What powerups have been chosen
	bool m_SpeedPowerupChosen = false;
	bool m_AccuracyPowerupChosen = false;
	bool m_DamagePowerupChosen = false;
	bool m_CooldownPowerupChosen = false;
	SDL_Color m_PowerupChoosingColour;

	// Loading screen
	Text m_LoadingText;
	bool m_IsOnLoadingScreen = false;

	// Number of points for a player to win
	unsigned int m_PointsToWin = SHORT_GAME_POINTS_TO_WIN;

	// Round over screen text
	Text m_ScoreCounterText;
	Text m_RedText;
	Text m_BlueText;
	Text m_GreyText;

private:
	// Initialises the start screen
	void initStartScreen();
	// Handles user input for start screen state
	void handleStartScreenEvents();
	// Updates the start screen
	void updateStartScreen();
	// Renders start screen page to the screen
	void drawStartScreen();

	// Initialises the gameplay state
	void initGameplay();
	// Handles user input for gameplay state
	void handleGameplayEvents();
	// Updates the gameplay
	void updateGameplay();
	// Renders gameplay to the screen
	void drawGameplay();

	// Initialises the round over state
	void initRoundOver();
	// Handles user input for round over state
	void handleRoundOverEvents();
	// Updates the round over state
	void updateRoundOver();
	// Renders round over state to the screen
	void drawRoundOver();

	// Draws the loading screen
	void drawLoadingScreen();

	// Initialises the players
	void initPlayers();

	// Resets the players
	void resetPlayers();

public:
	Game();
	~Game();

	// Runs the main-loop
	void run();
};
