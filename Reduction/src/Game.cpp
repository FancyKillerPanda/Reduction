#include "Game.h"

#include "utils/Settings.h"
#include "utils/Log.h"
#include "utils/MathUtils.h"
#include "utils/Random.h"


Game::Game()
{
	// Initialises SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		error("Could not initialise SDL.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	// Initialises TTF
	if (TTF_Init() != 0)
	{
		error("Could not initialise TTF.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	// Initialises Mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
	{
		error("Could not open Mixer audio.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3)\
	{
		error("Could not initialise Mixer.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	// Initialises the random generator
	Random::init();

	// Creates window
	m_Window = SDL_CreateWindow("Reduction", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!m_Window)
	{
		error("Could not create window.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	// Creates renderer
	m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);

	if (!m_Renderer)
	{
		error("Could not create renderer.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	// Sets the clear colour
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);

	// Sets up loading screen text
	m_LoadingText.load("res/fonts/SPACEMAN.TTF", "loading...", 56, SDL_Color { 255, 255, 255, 255 }, m_Renderer);

	// Initialises audio
	initAudio();

	initStartScreen();
	m_FrameTimer.reset();
}

Game::~Game()
{
	// Deletes players
	for (Player* player : m_Players)
	{
		delete player;
	}

	// Deletes buttons
	delete m_NextButton;
	delete m_TwoPlayersButton;
	delete m_ThreePlayersButton;
}


void Game::run()
{
	while (m_Running)
	{
		switch (m_GameState)
		{
		case GameState::StartScreen:
			handleStartScreenEvents();
			updateStartScreen();
			drawStartScreen();

			break;

		case GameState::Gameplay:
			handleGameplayEvents();
			updateGameplay();
			drawGameplay();

			break;

		case GameState::RoundOver:
			handleRoundOverEvents();
			updateRoundOver();
			drawRoundOver();

			break;

		case GameState::GameOver:
			handleGameOverEvents();
			updateGameOver();
			drawGameOver();

			break;
		}
	}
}


void Game::initGameplay()
{
	if (m_GameplayInitialised)
	{
		resetGameplayNewRound();
		return;
	}

	// Draws loading screen
	drawLoadingScreen();

	// Loads wall texture
	m_WallTexture = IMG_LoadTexture(m_Renderer, "res/txrs/Wall Mask.png");

	if (!m_WallTexture)
	{
		error("Could not load Wall texture.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	if (SDL_QueryTexture(m_WallTexture, nullptr, nullptr, &m_WallRect.w, &m_WallRect.h) != 0)
	{
		error("Wall texture is invalid.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	SDL_SetTextureAlphaMod(m_WallTexture, 60);

	m_OriginalWallWidth = m_WallRect.w;
	m_OriginalWallHeight = m_WallRect.h;

	// Loads background texture
	m_SpaceBackgroundTexture = IMG_LoadTexture(m_Renderer, "res/txrs/Space.png");

	if (!m_SpaceBackgroundTexture)
	{
		error("Could not load Space Background texture.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	if (SDL_QueryTexture(m_SpaceBackgroundTexture, nullptr, nullptr, &m_SpaceBackgroundRect.w, &m_SpaceBackgroundRect.h) != 0)
	{
		error("Space Background texture is invalid.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	m_SpaceBackgroundRect.w = SCREEN_WIDTH;
	m_SpaceBackgroundRect.h = SCREEN_HEIGHT;

	// Initialises barriers
	m_Barriers.push_back(Barrier { m_Renderer, SCREEN_WIDTH / 2 - BARRIER_DISTANCE_FROM_CENTER, SCREEN_HEIGHT / 2 - BARRIER_DISTANCE_FROM_CENTER });
	m_Barriers.push_back(Barrier { m_Renderer, SCREEN_WIDTH / 2 + BARRIER_DISTANCE_FROM_CENTER, SCREEN_HEIGHT / 2 - BARRIER_DISTANCE_FROM_CENTER });
	m_Barriers.push_back(Barrier { m_Renderer, SCREEN_WIDTH / 2 + BARRIER_DISTANCE_FROM_CENTER, SCREEN_HEIGHT / 2 + BARRIER_DISTANCE_FROM_CENTER });
	m_Barriers.push_back(Barrier { m_Renderer, SCREEN_WIDTH / 2 - BARRIER_DISTANCE_FROM_CENTER, SCREEN_HEIGHT / 2 + BARRIER_DISTANCE_FROM_CENTER });

	m_GameplayInitialised = true;

	m_FrameTimer.reset();
}

void Game::handleGameplayEvents()
{
	while (SDL_PollEvent(&m_Event))
	{
		switch (m_Event.type)
		{
		case SDL_QUIT:
			m_Running = false;
			break;

		case SDL_KEYDOWN:
			switch (m_Event.key.keysym.sym)
			{
			case SDLK_LEFT:
				if (m_Players[0]->isAlive())
				{
					m_Players[0]->setRotationSpeed(-PLAYER_ROTATION_SPEED);
				}

				break;

			case SDLK_RIGHT:
				if (m_Players[0]->isAlive())
				{
					m_Players[0]->setRotationSpeed(PLAYER_ROTATION_SPEED);
				}

				break;

			case SDLK_UP:
				if (m_Players[0]->isAlive())
				{
					m_Players[0]->setAcceleration(PLAYER_ACCELERATION);
				}

				break;

			case SDLK_DOWN:
				if (m_Players[0]->isAlive())
				{
					m_Players[0]->setAcceleration(-PLAYER_ACCELERATION * 2 / 3);
				}

				break;

			case SDLK_SLASH:
				if (m_Players[0]->isAlive())
				{
					m_Players[0]->spawnBullet();
				}

				break;

			case SDLK_a:
				if (m_Players[1]->isAlive())
				{
					m_Players[1]->setRotationSpeed(-PLAYER_ROTATION_SPEED);
				}

				break;

			case SDLK_d:
				if (m_Players[1]->isAlive())
				{
					m_Players[1]->setRotationSpeed(PLAYER_ROTATION_SPEED);
				}

				break;

			case SDLK_w:
				if (m_Players[1]->isAlive())
				{
					m_Players[1]->setAcceleration(PLAYER_ACCELERATION);
				}

				break;

			case SDLK_s:
				if (m_Players[1]->isAlive())
				{
					m_Players[1]->setAcceleration(-PLAYER_ACCELERATION * 2 / 3);
				}

				break;

			case SDLK_c:
				if (m_Players[1]->isAlive())
				{
					m_Players[1]->spawnBullet();
				}

				break;
			}

			break;

		case SDL_KEYUP:
			switch (m_Event.key.keysym.sym)
			{
			case SDLK_LEFT:
			case SDLK_RIGHT:
				if (m_Players[0]->isAlive())
				{
					m_Players[0]->setRotationSpeed(0.0);
				}

				break;

			case SDLK_UP:
			case SDLK_DOWN:
				if (m_Players[0]->isAlive())
				{
					m_Players[0]->setAcceleration(0.0);
				}

				break;

			case SDLK_a:
			case SDLK_d:
				if (m_Players[1]->isAlive())
				{
					m_Players[1]->setRotationSpeed(0.0);
				}

				break;

			case SDLK_w:
			case SDLK_s:
				if (m_Players[1]->isAlive())
				{
					m_Players[1]->setAcceleration(0.0);
				}

				break;
			}

			break;

		case SDL_MOUSEBUTTONDOWN:
			if (m_NumberOfPlayers == 3)
			{
				if (m_Event.button.button == SDL_BUTTON_LEFT)
				{
					if (m_Players[2]->isAlive())
					{
						m_Players[2]->spawnBullet();
					}
				}

				else if (m_Event.button.button == SDL_BUTTON_RIGHT)
				{
					if (m_Players[2]->isAlive())
					{
						m_Players[2]->setAcceleration(PLAYER_ACCELERATION);
					}
				}
			}

			break;

		case SDL_MOUSEBUTTONUP:
			if (m_NumberOfPlayers == 3)
			{
				if (m_Event.button.button == SDL_BUTTON_RIGHT)
				{
					if (m_Players[2]->isAlive())
					{
						m_Players[2]->setAcceleration(0.0);
					}
				}
			}
		}
	}
}

void Game::updateGameplay()
{
	// Number of seconds since last frame
	double dt = m_FrameTimer.getElapsed() / 1000;
	m_FrameTimer.reset();

	if (m_NumberOfPlayers == 3)
	{
		if (m_Players[2]->isAlive())
		{
			int mouseX;
			int mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);

			int deltaX = mouseX - m_Players[2]->getRect().x;
			int deltaY = mouseY - m_Players[2]->getRect().y;

			if (deltaX * deltaX + deltaY * deltaY > 100)
			{
				m_Players[2]->setRotation(toDegrees(std::atan2(deltaY, deltaX)));
			}

			else
			{
				m_Players[2]->setVelocity(0.0);
			}
		}
	}

	// Updates players
	for (Player* player : m_Players)
	{
		if (player->isAlive())
		{
			player->update(dt, m_WallScale);
			player->updateBullets(dt);
		}
	}

	// Checks for collisions between player and bullet
	for (unsigned int playerIndex = 0; playerIndex < m_Players.size(); playerIndex++)
	{
		Player* player = m_Players[playerIndex];

		if (!player->isAlive())
		{
			continue;
		}

		for (Player* otherPlayer : m_Players)
		{
			if (otherPlayer == player)
			{
				continue;
			}

			for (unsigned int bulletIndex = 0; bulletIndex < otherPlayer->getBullets().size(); bulletIndex++)
			{
				Bullet* bullet = otherPlayer->getBullets()[bulletIndex];

				if (SDL_HasIntersection(&player->getRect(), &bullet->getRect()))
				{
					player->takeHit(bullet);

					delete bullet;

					// Removes the bullet from the vector
					otherPlayer->getBullets().erase(otherPlayer->getBullets().begin() + bulletIndex);
					bulletIndex -= 1;
				}
			}
		}
	}

	// Updates wall
	if (m_WallScale >= 0.3 - WALL_SPEED)
	{
		m_WallScale += WALL_SPEED;
		m_WallRect.w = (int) (m_OriginalWallWidth * m_WallScale);
		m_WallRect.h = (int) (m_OriginalWallHeight * m_WallScale);
	}

	// Checks for end of game
	if (m_NumberOfPlayers == 2)
	{
		if ((int) m_Players[0]->isAlive() + (int) m_Players[1]->isAlive() <= 1)
		{
			m_GameState = GameState::RoundOver;
			initRoundOver();
		}
	}

	else
	{
		if ((int) m_Players[0]->isAlive() + (int) m_Players[1]->isAlive() + (int) m_Players[2]->isAlive() <= 1)
		{
			m_GameState = GameState::RoundOver;
			initRoundOver();
		}
	}
}

void Game::drawGameplay()
{
	SDL_RenderClear(m_Renderer);

	// Draws background
	SDL_RenderCopy(m_Renderer, m_SpaceBackgroundTexture, nullptr, &m_SpaceBackgroundRect);

	// Draws barriers
	for (Barrier& barrier : m_Barriers)
	{
		barrier.draw();
	}

	// Draws players
	for (Player* player : m_Players)
	{
		if (player->isAlive())
		{
			player->draw();
		}

		player->drawBullets();
	}

	// Sets the center of the wall opening to center of screen
	m_WallRect.x = (SCREEN_WIDTH / 2) - (m_WallRect.w / 2);
	m_WallRect.y = (SCREEN_HEIGHT / 2) - (m_WallRect.h / 2);

	// Draws wall
	SDL_RenderCopy(m_Renderer, m_WallTexture, nullptr, &m_WallRect);

	SDL_RenderPresent(m_Renderer);
}

void Game::resetGameplayNewRound()
{
	// Resets wall size
	m_WallRect.w = m_OriginalWallWidth;
	m_WallRect.h = m_OriginalWallHeight;
	m_WallScale = 1.0;

	// Resets frame timer
	m_FrameTimer.reset();
}


void Game::initStartScreen()
{
	// Sets blending mode
	SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);

	// Initialises header text
	m_ReductionText.load("res/fonts/SPACEMAN.TTF", "reduction", 56, SDL_Color { 255, 255, 255, 255 }, m_Renderer);
	m_ReductionText.setStyle(TTF_STYLE_BOLD, false);

	// Initialises buttons
	m_NextButton = new Button(m_Renderer, "-->");
	m_BackButton = new Button(m_Renderer, "<--");
	m_QuestionButton = new Button(m_Renderer, "?");
	m_TwoPlayersButton = new Button(m_Renderer, "Two Players");
	m_ThreePlayersButton = new Button(m_Renderer, "Three Players");
	m_SpeedPowerupButton = new Button(m_Renderer, "Speed Boost (15% Life)");
	m_AccuracyPowerupButton = new Button(m_Renderer, "Accuracy Boost (15% Life)");
	m_DamagePowerupButton = new Button(m_Renderer, "Damage Boost (15% Life)");
	m_CooldownPowerupButton = new Button(m_Renderer, "Cooldown Time Reduced (15% Life)");
	m_ShortGameButton = new Button(m_Renderer, "Short Game (3 Pt)");
	m_MediumGameButton = new Button(m_Renderer, "Medium Game (5 Pt)");
	m_LongGameButton = new Button(m_Renderer, "Long Game (7 Pt)");

	// Makes powerup button smaller
	m_SpeedPowerupButton->getText().setSize(16);
	m_AccuracyPowerupButton->getText().setSize(16);
	m_DamagePowerupButton->getText().setSize(16);
	m_CooldownPowerupButton->getText().setSize(16);

	// Makes game length buttons smaller
	m_ShortGameButton->getText().setSize(18);
	m_MediumGameButton->getText().setSize(18);
	m_LongGameButton->getText().setSize(18);

	m_StartScreenPage = StartScreenPage::NumberOfPlayersChoice;
	m_StartScreenInitialised = true;

	// Loads background texture
	m_SpaceBackgroundTexture = IMG_LoadTexture(m_Renderer, "res/txrs/Start Screen Space.jpg");

	if (!m_SpaceBackgroundTexture)
	{
		error("Could not load Space Background texture.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	if (SDL_QueryTexture(m_SpaceBackgroundTexture, nullptr, nullptr, &m_SpaceBackgroundRect.w, &m_SpaceBackgroundRect.h) != 0)
	{
		error("Space Background texture is invalid.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	m_SpaceBackgroundRect.w = SCREEN_WIDTH;
	m_SpaceBackgroundRect.h = SCREEN_HEIGHT;

	SDL_SetTextureColorMod(m_SpaceBackgroundTexture, 127, 127, 127);

	// Powerup textures
	m_SpeedPowerupTexture = IMG_LoadTexture(m_Renderer, "res/txrs/Bolt.png");
	m_AccuracyPowerupTexture = IMG_LoadTexture(m_Renderer, "res/txrs/Crosshairs.png");
	m_DamagePowerupTexture = IMG_LoadTexture(m_Renderer, "res/txrs/Heart.png");
	m_CooldownPowerupTexture = IMG_LoadTexture(m_Renderer, "res/txrs/Stopwatch.png");

	if (!(m_SpeedPowerupTexture && m_AccuracyPowerupTexture && m_DamagePowerupTexture && m_CooldownPowerupTexture))
	{
		error("Could not load power-up texture.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	if (
		SDL_QueryTexture(m_SpeedPowerupTexture, nullptr, nullptr, &m_SpeedPowerupRect.w, &m_SpeedPowerupRect.h) != 0 ||
		SDL_QueryTexture(m_AccuracyPowerupTexture, nullptr, nullptr, &m_AccuracyPowerupRect.w, &m_AccuracyPowerupRect.h) != 0 ||
		SDL_QueryTexture(m_DamagePowerupTexture, nullptr, nullptr, &m_DamagePowerupRect.w, &m_DamagePowerupRect.h) != 0 ||
		SDL_QueryTexture(m_CooldownPowerupTexture, nullptr, nullptr, &m_CooldownPowerupRect.w, &m_CooldownPowerupRect.h) != 0
		)
	{
		error("Power-up texture is invalid.\nSDL_Error: ", SDL_GetError());
		m_Running = false;

		return;
	}

	m_SpeedPowerupRect.w = 64;
	m_SpeedPowerupRect.h = 64;
	m_SpeedPowerupRect.x = SCREEN_WIDTH / 4 - m_SpeedPowerupRect.w / 2;
	m_SpeedPowerupRect.y = SCREEN_HEIGHT * 9 / 20 - m_SpeedPowerupRect.h / 2;

	m_AccuracyPowerupRect.w = 64;
	m_AccuracyPowerupRect.h = 64;
	m_AccuracyPowerupRect.x = SCREEN_WIDTH / 4 - m_SpeedPowerupRect.w / 2;
	m_AccuracyPowerupRect.y = SCREEN_HEIGHT * 13 / 20 - m_SpeedPowerupRect.h / 2;

	m_DamagePowerupRect.w = 64;
	m_DamagePowerupRect.h = 64;
	m_DamagePowerupRect.x = SCREEN_WIDTH * 3 / 4 - m_SpeedPowerupRect.w / 2;
	m_DamagePowerupRect.y = SCREEN_HEIGHT * 9 / 20 - m_SpeedPowerupRect.h / 2;

	m_CooldownPowerupRect.w = 64;
	m_CooldownPowerupRect.h = 64;
	m_CooldownPowerupRect.x = SCREEN_WIDTH * 3 / 4 - m_SpeedPowerupRect.w / 2;
	m_CooldownPowerupRect.y = SCREEN_HEIGHT * 13 / 20 - m_SpeedPowerupRect.h / 2;

	// Initialises powerups questions
	m_RedPowerupsText.load("res/fonts/BM Space.TTF", "Red Player, what powerups would you like?", 18, SDL_Colour { 255, 0, 0, 255 }, m_Renderer);
	m_RedPowerupsText.setStyle(TTF_STYLE_BOLD);
	m_BluePowerupsText.load("res/fonts/BM Space.TTF", "Blue Player, what powerups would you like?", 18, SDL_Colour { 0, 0, 255, 255 }, m_Renderer);
	m_BluePowerupsText.setStyle(TTF_STYLE_BOLD);
	m_GreyPowerupsText.load("res/fonts/BM Space.TTF", "Grey Player, what powerups would you like?", 18, SDL_Colour { 127, 127, 127, 255 }, m_Renderer);
	m_GreyPowerupsText.setStyle(TTF_STYLE_BOLD);

	// Current colour selecting powerups (red)
	m_PowerupChoosingColour = SDL_Color { 255, 0, 0, 255 };

	// Initialises general help texts
	m_HelpGeneralTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "Reduction is a two or three player space-shooter game.", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpGeneralTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "It has a wall that gradually encloses the players,", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpGeneralTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "with the players losing life faster when they move", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpGeneralTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "further out. Each player can buy powerups before each", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpGeneralTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "round for a portion of their life, which could help them", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpGeneralTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "win the game.", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);

	// Initialises controls help text
	m_HelpControlsTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "Red Player", 18, SDL_Color { 255, 0, 0, 255 }, m_Renderer)
	);
	m_HelpControlsTexts.back()->setStyle(TTF_STYLE_BOLD);
	m_HelpControlsTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "<Up> / <Down> -> Accelerate / Brake", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpControlsTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "<Left> / <Right> -> Rotate", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpControlsTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "<C> -> Shoot", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpControlsTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "Blue Player", 18, SDL_Color { 0, 0, 255, 255 }, m_Renderer)
	);
	m_HelpControlsTexts.back()->setStyle(TTF_STYLE_BOLD);
	m_HelpControlsTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "<W> / <S> -> Accelerate / Brake", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpControlsTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "<A> / <D> -> Rotate", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpControlsTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "</> -> Shoot", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpControlsTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "Grey Player", 18, SDL_Color { 127, 127, 127, 255 }, m_Renderer)
	);
	m_HelpControlsTexts.back()->setStyle(TTF_STYLE_BOLD);
	m_HelpControlsTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "<Right Click> -> Accelerate", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpControlsTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "<Move Cursor> -> Rotate", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);
	m_HelpControlsTexts.push_back(
		new Text("res/fonts/BM Space.TTF", "<Left Click> -> Shoot", 18, SDL_Color { 255, 255, 255, 255 }, m_Renderer)
	);

	m_FrameTimer.reset();
}

void Game::handleStartScreenEvents()
{
	while (SDL_PollEvent(&m_Event))
	{
		switch (m_Event.type)
		{
		case SDL_QUIT:
			m_Running = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
			switch (m_StartScreenPage)
			{
			case StartScreenPage::NumberOfPlayersChoice:
				if (m_TwoPlayersButton->isMouseOver())
				{
					m_NumberOfPlayers = 2;
					m_StartScreenPage = StartScreenPage::GameLengthChoice;
				}

				else if (m_ThreePlayersButton->isMouseOver())
				{
					m_NumberOfPlayers = 3;
					m_StartScreenPage = StartScreenPage::GameLengthChoice;
				}

				else if (m_QuestionButton->isMouseOver())
				{
					m_StartScreenPage = StartScreenPage::HelpGeneral;
				}

				break;

			case StartScreenPage::GameLengthChoice:
				if (m_BackButton->isMouseOver())
				{
					m_StartScreenPage = StartScreenPage::NumberOfPlayersChoice;
				}

				else if (m_ShortGameButton->isMouseOver())
				{
					m_PointsToWin = SHORT_GAME_POINTS_TO_WIN;

					m_StartScreenPage = StartScreenPage::RedPowerUp;
					initPlayers();
				}

				else if (m_MediumGameButton->isMouseOver())
				{
					m_PointsToWin = MEDIUM_GAME_POINTS_TO_WIN;

					m_StartScreenPage = StartScreenPage::RedPowerUp;
					initPlayers();
				}

				else if (m_LongGameButton->isMouseOver())
				{
					m_PointsToWin = LONG_GAME_POINTS_TO_WIN;

					m_StartScreenPage = StartScreenPage::RedPowerUp;
					initPlayers();
				}

				break;

			case StartScreenPage::RedPowerUp:
				if (m_NextButton->isMouseOver())
				{
					// Sets the powerups for the red player
					m_Players[0]->setPowerups(m_SpeedPowerupChosen, m_AccuracyPowerupChosen, m_DamagePowerupChosen, m_CooldownPowerupChosen);

					m_StartScreenPage = StartScreenPage::BluePowerUp;

					m_SpeedPowerupChosen = false;
					m_AccuracyPowerupChosen = false;
					m_DamagePowerupChosen = false;
					m_CooldownPowerupChosen = false;

					m_PowerupChoosingColour = SDL_Color { 0, 0, 255, 255 };
				}

				else if (m_BackButton->isMouseOver())
				{
					m_StartScreenPage = StartScreenPage::GameLengthChoice;
				}

				break;

			case StartScreenPage::BluePowerUp:
				if (m_NextButton->isMouseOver())
				{
					// Sets the powerups for the blue player
					m_Players[1]->setPowerups(m_SpeedPowerupChosen, m_AccuracyPowerupChosen, m_DamagePowerupChosen, m_CooldownPowerupChosen);

					if (m_NumberOfPlayers == 2)
					{
						m_GameState = GameState::Gameplay;
						initGameplay();
					}

					else
					{
						m_StartScreenPage = StartScreenPage::GreyPowerUp;

						m_SpeedPowerupChosen = false;
						m_AccuracyPowerupChosen = false;
						m_DamagePowerupChosen = false;
						m_CooldownPowerupChosen = false;

						m_PowerupChoosingColour = SDL_Color { 127, 127, 127, 255 };
					}
				}

				break;

			case StartScreenPage::GreyPowerUp:
				if (m_NextButton->isMouseOver())
				{
					// Sets the powerups for the grey player
					m_Players[2]->setPowerups(m_SpeedPowerupChosen, m_AccuracyPowerupChosen, m_DamagePowerupChosen, m_CooldownPowerupChosen);

					m_GameState = GameState::Gameplay;
					initGameplay();
				}

				break;

			case StartScreenPage::HelpGeneral:
				if (m_NextButton->isMouseOver())
				{
					m_StartScreenPage = StartScreenPage::HelpControls;
				}

				else if (m_BackButton->isMouseOver())
				{
					m_StartScreenPage = StartScreenPage::NumberOfPlayersChoice;
				}

				break;

			case StartScreenPage::HelpControls:
				if (m_BackButton->isMouseOver())
				{
					m_StartScreenPage = StartScreenPage::HelpGeneral;
				}

				break;

			default:
				break;
			}

			if (m_StartScreenPage == StartScreenPage::RedPowerUp ||
				m_StartScreenPage == StartScreenPage::BluePowerUp ||
				m_StartScreenPage == StartScreenPage::GreyPowerUp)
			{
				// Gets mouse position
				int mouseX;
				int mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
				SDL_Rect mousePos = { mouseX, mouseY, 1, 1 };

				if (m_SpeedPowerupButton->isMouseOver() || SDL_HasIntersection(&mousePos, &m_SpeedPowerupRect))
				{
					m_SpeedPowerupChosen = !m_SpeedPowerupChosen;
				}

				else if (m_AccuracyPowerupButton->isMouseOver() || SDL_HasIntersection(&mousePos, &m_AccuracyPowerupRect))
				{
					m_AccuracyPowerupChosen = !m_AccuracyPowerupChosen;
				}

				else if (m_DamagePowerupButton->isMouseOver() || SDL_HasIntersection(&mousePos, &m_DamagePowerupRect))
				{
					m_DamagePowerupChosen = !m_DamagePowerupChosen;
				}

				else if (m_CooldownPowerupButton->isMouseOver() || SDL_HasIntersection(&mousePos, &m_CooldownPowerupRect))
				{
					m_CooldownPowerupChosen = !m_CooldownPowerupChosen;
				}
			}

			break;
		}
	}
}

void Game::updateStartScreen()
{
	switch (m_StartScreenPage)
	{
	case StartScreenPage::NumberOfPlayersChoice:
		m_QuestionButton->update();
		m_TwoPlayersButton->update();
		m_ThreePlayersButton->update();

		break;

	case StartScreenPage::GameLengthChoice:
		m_BackButton->update();
		m_ShortGameButton->update();
		m_MediumGameButton->update();
		m_LongGameButton->update();

		break;

	case StartScreenPage::RedPowerUp:
		m_BackButton->update();
	case StartScreenPage::BluePowerUp:
	case StartScreenPage::GreyPowerUp:
		m_SpeedPowerupButton->update();
		m_AccuracyPowerupButton->update();
		m_DamagePowerupButton->update();
		m_CooldownPowerupButton->update();
		m_NextButton->update();

		break;

	case StartScreenPage::HelpGeneral:
		m_NextButton->update();
		m_BackButton->update();
		break;

	case StartScreenPage::HelpControls:
		m_BackButton->update();

		break;

	default:
		break;
	}
}

void Game::drawStartScreen()
{
	if (m_IsOnLoadingScreen)
	{
		m_IsOnLoadingScreen = false;
		return;
	}

	SDL_RenderClear(m_Renderer);

	// Draws background
	SDL_RenderCopy(m_Renderer, m_SpaceBackgroundTexture, nullptr, &m_SpaceBackgroundRect);

	switch (m_StartScreenPage)
	{
	case StartScreenPage::NumberOfPlayersChoice:
		m_ReductionText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 5 / 20);
		m_QuestionButton->draw(SCREEN_WIDTH * 18 / 20, SCREEN_HEIGHT * 18 / 20);
		m_TwoPlayersButton->draw(SCREEN_WIDTH * 6 / 20, SCREEN_HEIGHT * 11 / 20);
		m_ThreePlayersButton->draw(SCREEN_WIDTH * 14 / 20, SCREEN_HEIGHT * 11 / 20);

		break;

	case StartScreenPage::GameLengthChoice:
		m_ReductionText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 5 / 20);
		m_BackButton->draw(SCREEN_WIDTH * 1 / 8, SCREEN_HEIGHT * 7 / 8);
		m_ShortGameButton->draw(SCREEN_WIDTH * 4 / 20, SCREEN_HEIGHT * 11 / 20);
		m_MediumGameButton->draw(SCREEN_WIDTH * 10 / 20, SCREEN_HEIGHT * 11 / 20);
		m_LongGameButton->draw(SCREEN_WIDTH * 16 / 20, SCREEN_HEIGHT * 11 / 20);

		break;

	case StartScreenPage::RedPowerUp:
		m_BackButton->draw(SCREEN_WIDTH * 1 / 8, SCREEN_HEIGHT * 7 / 8);
	case StartScreenPage::BluePowerUp:
	case StartScreenPage::GreyPowerUp:
		m_ReductionText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 1 / 5);
		m_NextButton->draw(SCREEN_WIDTH * 7 / 8, SCREEN_HEIGHT * 7 / 8);

		if (m_SpeedPowerupChosen)
		{
			SDL_SetRenderDrawColor(m_Renderer, m_PowerupChoosingColour.r, m_PowerupChoosingColour.g, m_PowerupChoosingColour.b, 127);
			SDL_RenderFillRect(m_Renderer, &m_SpeedPowerupRect);
		}

		if (m_AccuracyPowerupChosen)
		{
			SDL_SetRenderDrawColor(m_Renderer, m_PowerupChoosingColour.r, m_PowerupChoosingColour.g, m_PowerupChoosingColour.b, 127);
			SDL_RenderFillRect(m_Renderer, &m_AccuracyPowerupRect);
		}

		if (m_DamagePowerupChosen)
		{
			SDL_SetRenderDrawColor(m_Renderer, m_PowerupChoosingColour.r, m_PowerupChoosingColour.g, m_PowerupChoosingColour.b, 127);
			SDL_RenderFillRect(m_Renderer, &m_DamagePowerupRect);
		}

		if (m_CooldownPowerupChosen)
		{
			SDL_SetRenderDrawColor(m_Renderer, m_PowerupChoosingColour.r, m_PowerupChoosingColour.g, m_PowerupChoosingColour.b, 127);
			SDL_RenderFillRect(m_Renderer, &m_CooldownPowerupRect);
		}

		SDL_RenderCopy(m_Renderer, m_SpeedPowerupTexture, nullptr, &m_SpeedPowerupRect);
		m_SpeedPowerupButton->draw(SCREEN_WIDTH / 4, SCREEN_HEIGHT * 7 / 20);
		SDL_RenderCopy(m_Renderer, m_AccuracyPowerupTexture, nullptr, &m_AccuracyPowerupRect);
		m_AccuracyPowerupButton->draw(SCREEN_WIDTH / 4, SCREEN_HEIGHT * 15 / 20);
		SDL_RenderCopy(m_Renderer, m_DamagePowerupTexture, nullptr, &m_DamagePowerupRect);
		m_DamagePowerupButton->draw(SCREEN_WIDTH * 3 / 4, SCREEN_HEIGHT * 7 / 20);
		SDL_RenderCopy(m_Renderer, m_CooldownPowerupTexture, nullptr, &m_CooldownPowerupRect);
		m_CooldownPowerupButton->draw(SCREEN_WIDTH * 3 / 4, SCREEN_HEIGHT * 15 / 20);

		break;

	case StartScreenPage::HelpGeneral:
	{
		m_ReductionText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 5 / 20);
		m_NextButton->draw(SCREEN_WIDTH * 7 / 8, SCREEN_HEIGHT * 7 / 8);
		m_BackButton->draw(SCREEN_WIDTH * 1 / 8, SCREEN_HEIGHT * 7 / 8);

		unsigned int y = 9;

		for (Text* text : m_HelpGeneralTexts)
		{
			text->draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * y / 20);
			y += 1;
		}

		break;
	}

	case StartScreenPage::HelpControls:
	{
		m_ReductionText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 3 / 20);
		m_BackButton->draw(SCREEN_WIDTH * 1 / 8, SCREEN_HEIGHT * 7 / 8);

		unsigned int y = 6;

		for (Text* text : m_HelpControlsTexts)
		{
			text->draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * y / 20);
			y += 1;
		}

		break;
	}

	default:
		break;
	}

	switch (m_StartScreenPage)
	{
	case StartScreenPage::RedPowerUp:
		m_RedPowerupsText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7 / 8);
		break;

	case StartScreenPage::BluePowerUp:
		m_BluePowerupsText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7 / 8);
		break;

	case StartScreenPage::GreyPowerUp:
		m_GreyPowerupsText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 7 / 8);
		break;

	default:
		break;
	}

	SDL_RenderPresent(m_Renderer);
}

void Game::resetStartScreenNewRound()
{
	// Current colour selecting powerups (red)
	m_PowerupChoosingColour = SDL_Color { 255, 0, 0, 255 };
}


void Game::initRoundOver()
{
	SDL_Color winningColour;

	if (!m_Players[0]->isAlive() && !m_Players[1]->isAlive() && !(m_NumberOfPlayers == 3 && m_Players[2]->isAlive()))
	{
		winningColour = SDL_Color { 255, 255, 255, 255 };
	}

	else if (m_Players[0]->isAlive())
	{
		m_Players[0]->addPoint();
		winningColour = SDL_Color { 255, 0, 0, 255 };
	}

	else if (m_Players[1]->isAlive())
	{
		m_Players[1]->addPoint();
		winningColour = SDL_Color { 0, 0, 255, 255 };
	}

	else if (m_NumberOfPlayers == 3 && m_Players[2]->isAlive())
	{
		m_Players[2]->addPoint();
		winningColour = SDL_Color { 127, 127, 127, 255 };
	}

	if (m_Players[0]->getPoints() == m_PointsToWin ||
		m_Players[1]->getPoints() == m_PointsToWin ||
		(m_NumberOfPlayers == 3 && m_Players[2]->getPoints() == m_PointsToWin))
	{
		m_GameState = GameState::GameOver;
		initGameOver();
		return;
	}

	resetPlayers();

	std::string scoreText = std::to_string(m_Players[0]->getPoints()) + " - " + std::to_string(m_Players[1]->getPoints());

	if (m_NumberOfPlayers == 3)
	{
		scoreText += " - " + std::to_string(m_Players[2]->getPoints());
	}

	m_ScoreCounterText.load("res/fonts/BM Space.TTF", scoreText, 48, winningColour, m_Renderer);
	m_RedText.load("res/fonts/BM Space.TTF", "Red", 16, SDL_Color { 255, 0, 0, 255 }, m_Renderer);
	m_BlueText.load("res/fonts/BM Space.TTF", "Blue", 16, SDL_Color { 0, 0, 255, 255 }, m_Renderer);

	if (m_NumberOfPlayers == 3)
	{
		m_GreyText.load("res/fonts/BM Space.TTF", "Grey", 16, SDL_Color { 127, 127, 127, 255 }, m_Renderer);
	}
}

void Game::handleRoundOverEvents()
{
	while (SDL_PollEvent(&m_Event))
	{
		switch (m_Event.type)
		{
		case SDL_QUIT:
			m_Running = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (m_NextButton->isMouseOver())
			{
				m_GameState = GameState::StartScreen;
				m_StartScreenPage = StartScreenPage::RedPowerUp;

				resetStartScreenNewRound();
			}

			break;
		}
	}
}

void Game::updateRoundOver()
{
	m_NextButton->update();
}

void Game::drawRoundOver()
{
	SDL_RenderClear(m_Renderer);

	SDL_RenderCopy(m_Renderer, m_SpaceBackgroundTexture, nullptr, &m_SpaceBackgroundRect);
	m_ReductionText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 5 / 20);
	m_ScoreCounterText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 10 / 20);

	if (m_NumberOfPlayers == 2)
	{
		m_RedText.draw(SCREEN_WIDTH * 8 / 20, SCREEN_HEIGHT * 12 / 20);
		m_BlueText.draw(SCREEN_WIDTH * 12 / 20, SCREEN_HEIGHT * 12 / 20);
	}

	else
	{
		m_RedText.draw(SCREEN_WIDTH * 6 / 20, SCREEN_HEIGHT * 12 / 20);
		m_BlueText.draw(SCREEN_WIDTH * 10 / 20, SCREEN_HEIGHT * 12 / 20);
		m_GreyText.draw(SCREEN_WIDTH * 14 / 20, SCREEN_HEIGHT * 12 / 20);
	}

	m_NextButton->draw(SCREEN_WIDTH * 7 / 8, SCREEN_HEIGHT * 7 / 8);

	SDL_RenderPresent(m_Renderer);
}


void Game::initGameOver()
{
	// Who wins
	std::string winner;
	SDL_Color winningColour;

	if (m_Players[0]->getPoints() == m_PointsToWin)
	{
		winner += "Red";
		winningColour = SDL_Color { 255, 0, 0, 255 };
	}

	else if (m_Players[1]->getPoints() == m_PointsToWin)
	{
		winner += "Blue";
		winningColour = SDL_Color { 0, 0, 255, 255 };
	}

	else if (m_NumberOfPlayers == 3 &&  m_Players[2]->getPoints() == m_PointsToWin)
	{
		winner += "Grey";
		winningColour = SDL_Color { 127, 127, 127, 255 };
	}

	winner += " Wins!";

	// Loads the winner text
	m_WinnerText.load("res/fonts/BM Space.TTF", winner, 48, winningColour, m_Renderer);

	// Score counter
	std::string scoreText = std::to_string(m_Players[0]->getPoints()) + " - " + std::to_string(m_Players[1]->getPoints());

	if (m_NumberOfPlayers == 3)
	{
		scoreText += " - " + std::to_string(m_Players[2]->getPoints());
	}

	m_ScoreCounterText.load("res/fonts/BM Space.TTF", scoreText, 48, winningColour, m_Renderer);
	m_RedText.load("res/fonts/BM Space.TTF", "Red", 16, SDL_Color { 255, 0, 0, 255 }, m_Renderer);
	m_BlueText.load("res/fonts/BM Space.TTF", "Blue", 16, SDL_Color { 0, 0, 255, 255 }, m_Renderer);

	if (m_NumberOfPlayers == 3)
	{
		m_GreyText.load("res/fonts/BM Space.TTF", "Grey", 16, SDL_Color { 127, 127, 127, 255 }, m_Renderer);
	}
}

void Game::handleGameOverEvents()
{
	while (SDL_PollEvent(&m_Event))
	{
		switch (m_Event.type)
		{
		case SDL_QUIT:
			m_Running = false;
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (m_NextButton->isMouseOver())
			{
				m_GameState = GameState::StartScreen;
				m_StartScreenPage = StartScreenPage::NumberOfPlayersChoice;
				resetPlayers(true);
			}

			break;
		}
	}
}

void Game::updateGameOver()
{
	m_NextButton->update();
}

void Game::drawGameOver()
{
	SDL_RenderClear(m_Renderer);

	SDL_RenderCopy(m_Renderer, m_SpaceBackgroundTexture, nullptr, &m_SpaceBackgroundRect);
	m_ReductionText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 5 / 20);
	m_ScoreCounterText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 9 / 20);

	if (m_NumberOfPlayers == 2)
	{
		m_RedText.draw(SCREEN_WIDTH * 8 / 20, SCREEN_HEIGHT * 11 / 20);
		m_BlueText.draw(SCREEN_WIDTH * 12 / 20, SCREEN_HEIGHT * 11 / 20);
	}

	else
	{
		m_RedText.draw(SCREEN_WIDTH * 6 / 20, SCREEN_HEIGHT * 11 / 20);
		m_BlueText.draw(SCREEN_WIDTH * 10 / 20, SCREEN_HEIGHT * 11 / 20);
		m_GreyText.draw(SCREEN_WIDTH * 14 / 20, SCREEN_HEIGHT * 11 / 20);
	}

	m_WinnerText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 14 / 20);

	m_NextButton->draw(SCREEN_WIDTH * 7 / 8, SCREEN_HEIGHT * 7 / 8);

	SDL_RenderPresent(m_Renderer);
}


void Game::initPlayers()
{
	// Removes old players if they exist
	for (Player* player : m_Players)
	{
		delete player;
	}

	m_Players.clear();

	// Initialises the players
	m_Players.push_back(new Player(m_Renderer, PlayerColour::Red, RED_PLAYER_START_X, RED_PLAYER_START_Y, RED_PLAYER_START_DIRECTION));
	m_Players.push_back(new Player(m_Renderer, PlayerColour::Blue, BLUE_PLAYER_START_X, BLUE_PLAYER_START_Y, BLUE_PLAYER_START_DIRECTION));

	if (m_NumberOfPlayers == 3)
	{
		m_Players.push_back(new Player(m_Renderer, PlayerColour::Grey, GREY_PLAYER_START_X, GREY_PLAYER_START_Y, GREY_PLAYER_START_DIRECTION));
	}
}

void Game::drawLoadingScreen()
{
	m_IsOnLoadingScreen = true;

	SDL_RenderClear(m_Renderer);
	SDL_RenderCopy(m_Renderer, m_SpaceBackgroundTexture, nullptr, &m_SpaceBackgroundRect);
	m_LoadingText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	SDL_RenderPresent(m_Renderer);
}

void Game::resetPlayers(bool completeReset)
{
	for (Player* player : m_Players)
	{
		player->reset(completeReset);
		player->setLifeLeft((int) PLAYER_STARTING_LIFE); // BUG: Should be done in reset(), doesn't work
	}
}

void Game::initAudio()
{
	m_BackgroundMusic = Mix_LoadMUS("res/audio/Deep Space.mp3");
	Mix_VolumeMusic(32);
	Mix_PlayMusic(m_BackgroundMusic, -1);

	Player::initAudio();
}
