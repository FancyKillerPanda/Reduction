#include "Game.h"

#include "utils/Settings.h"
#include "utils/Log.h"
#include "utils/MathUtils.h"


Game::Game()
{
	// Initialises SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
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
		}
	}
}


void Game::initGameplay()
{
	// Initialises the players
	m_Players.push_back(new Player(m_Renderer, PlayerColour::Red, (SCREEN_WIDTH / 2) - (SCREEN_HEIGHT / 2) + 40, SCREEN_HEIGHT / 2, 270));
	m_Players.push_back(new Player(m_Renderer, PlayerColour::Blue, (SCREEN_WIDTH / 2) + (SCREEN_HEIGHT / 2) - 40, SCREEN_HEIGHT / 2, 90));

	if (m_NumberOfPlayers == 3)
	{
		m_Players.push_back(new Player(m_Renderer, PlayerColour::Grey, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 40, 180));
		m_Players[2]->setAcceleration(PLAYER_ACCELERATION);
	}

	// Loads wall texture
	m_WallTexture = IMG_LoadTexture(m_Renderer, "res/Wall Mask.png");

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
	m_SpaceBackgroundTexture = IMG_LoadTexture(m_Renderer, "res/Space.png");

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
				m_Players[0]->setRotationSpeed(-PLAYER_ROTATION_SPEED);
				break;

			case SDLK_RIGHT:
				m_Players[0]->setRotationSpeed(PLAYER_ROTATION_SPEED);
				break;

			case SDLK_UP:
				m_Players[0]->setAcceleration(PLAYER_ACCELERATION);
				break;

			case SDLK_DOWN:
				m_Players[0]->setAcceleration(-PLAYER_ACCELERATION * 2 / 3);
				break;

			case SDLK_SLASH:
				m_Players[0]->spawnBullet();
				break;

			case SDLK_a:
				m_Players[1]->setRotationSpeed(-PLAYER_ROTATION_SPEED);
				break;

			case SDLK_d:
				m_Players[1]->setRotationSpeed(PLAYER_ROTATION_SPEED);
				break;

			case SDLK_w:
				m_Players[1]->setAcceleration(PLAYER_ACCELERATION);
				break;

			case SDLK_s:
				m_Players[1]->setAcceleration(-PLAYER_ACCELERATION * 2 / 3);
				break;

			case SDLK_c:
				m_Players[1]->spawnBullet();
				break;
			}

			break;

		case SDL_KEYUP:
			switch (m_Event.key.keysym.sym)
			{
			case SDLK_LEFT:
			case SDLK_RIGHT:
				m_Players[0]->setRotationSpeed(0.0);
				break;

			case SDLK_UP:
			case SDLK_DOWN:
				m_Players[0]->setAcceleration(0.0);
				break;

			case SDLK_a:
			case SDLK_d:
				m_Players[1]->setRotationSpeed(0.0);
				break;

			case SDLK_w:
			case SDLK_s:
				m_Players[1]->setAcceleration(0.0);
				break;
			}

			break;

		case SDL_MOUSEBUTTONDOWN:
			if (m_NumberOfPlayers == 3)
			{
				m_Players[2]->spawnBullet();
			}

			break;
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
		int mouseX;
		int mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		int deltaX = mouseX - m_Players[2]->getRect().x;
		int deltaY = mouseY - m_Players[2]->getRect().y;

		if (deltaX * deltaX + deltaY * deltaY > 100)
		{
			m_Players[2]->setRotation(toDegrees(std::atan2(deltaY, deltaX)));
			m_Players[2]->setAcceleration(PLAYER_ACCELERATION);
		}

		else
		{
			m_Players[2]->setVelocity(0.0);
		}
	}

	// Updates players
	for (Player* player : m_Players)
	{
		player->update(dt, m_WallScale);
		player->updateBullets(dt);
	}

	// Checks for collisions between player and bullet
	for (unsigned int playerIndex = 0; playerIndex < m_Players.size(); playerIndex++)
	{
		Player* player = m_Players[playerIndex];

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
					delete bullet;

					// Removes the bullet from the vector
					otherPlayer->getBullets().erase(otherPlayer->getBullets().begin() + bulletIndex);
					bulletIndex -= 1;

					player->takeHit();
				}
			}
		}

		if (player->getLifeLeft() == 0)
		{
			delete player;

			m_Players.erase(m_Players.begin() + playerIndex);
			playerIndex -= 1;
		}
	}

	// Updates wall
	if (m_WallScale >= 0.3 - WALL_SPEED)
	{
		m_WallScale += WALL_SPEED;
		m_WallRect.w = (int) (m_OriginalWallWidth * m_WallScale);
		m_WallRect.h = (int) (m_OriginalWallHeight * m_WallScale);
	}

}

void Game::drawGameplay()
{
	SDL_RenderClear(m_Renderer);

	// Draws background
	SDL_RenderCopy(m_Renderer, m_SpaceBackgroundTexture, nullptr, &m_SpaceBackgroundRect);

	// Draws players
	for (Player* player : m_Players)
	{
		player->draw();
		player->drawBullets();
	}

	// Sets the center of the wall opening to center of screen
	m_WallRect.x = (SCREEN_WIDTH / 2) - (m_WallRect.w / 2);
	m_WallRect.y = (SCREEN_HEIGHT / 2) - (m_WallRect.h / 2);

	// Draws wall
	SDL_RenderCopy(m_Renderer, m_WallTexture, nullptr, &m_WallRect);

	SDL_RenderPresent(m_Renderer);
}


void Game::initStartScreen()
{
	// Sets blending mode
	SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);

	// Initialises header text
	m_ReductionText.load("res/SPACEMAN.TTF", "reduction", 56, SDL_Color { 255, 255, 255, 255 }, m_Renderer);
	m_ReductionText.setStyle(TTF_STYLE_BOLD, false);

	// Initialises buttons
	m_NextButton = new Button(m_Renderer, "-->");
	m_TwoPlayersButton = new Button(m_Renderer, "Two Players");
	m_ThreePlayersButton = new Button(m_Renderer, "Three Players");
	m_SpeedPowerupButton = new Button(m_Renderer, "Speed Boost | 15% Life");
	m_AccuracyPowerupButton = new Button(m_Renderer, "Accuracy Boost | 15% Life");
	m_DamagePowerupButton = new Button(m_Renderer, "Damage Boost | 15% Life");
	m_CooldownPowerupButton = new Button(m_Renderer, "Cooldown Time Reduced | 15% Life");

	// Makes powerup button smaller
	m_SpeedPowerupButton->getText().setSize(16);
	m_AccuracyPowerupButton->getText().setSize(16);
	m_DamagePowerupButton->getText().setSize(16);
	m_CooldownPowerupButton->getText().setSize(16);

	m_StartScreenPage = StartScreenPage::NumberOfPlayersChoice;
	m_StartScreenInitialised = true;

	// Loads background texture
	m_SpaceBackgroundTexture = IMG_LoadTexture(m_Renderer, "res/Start Screen Space.jpg");

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

	// Powerup textures
	m_SpeedPowerupTexture = IMG_LoadTexture(m_Renderer, "res/Bolt.png");
	m_AccuracyPowerupTexture = IMG_LoadTexture(m_Renderer, "res/Crosshairs.png");
	m_DamagePowerupTexture = IMG_LoadTexture(m_Renderer, "res/Heart.png");
	m_CooldownPowerupTexture = IMG_LoadTexture(m_Renderer, "res/Stopwatch.png");

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
	m_RedPowerupsText.load("res/BM Space.TTF", "Red Player, what powerups would you like?", 18, SDL_Colour { 255, 0, 0, 255 }, m_Renderer);
	m_RedPowerupsText.setStyle(TTF_STYLE_BOLD);
	m_BluePowerupsText.load("res/BM Space.TTF", "Blue Player, what powerups would you like?", 18, SDL_Colour { 0, 0, 255, 255 }, m_Renderer);
	m_BluePowerupsText.setStyle(TTF_STYLE_BOLD);
	m_GreyPowerupsText.load("res/BM Space.TTF", "Grey Player, what powerups would you like?", 18, SDL_Colour { 127, 127, 127, 255 }, m_Renderer);
	m_GreyPowerupsText.setStyle(TTF_STYLE_BOLD);

	// Current colour selecting powerups (red)
	m_PowerupChoosingColour = SDL_Color { 255, 0, 0, 255 };

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
					m_StartScreenPage = StartScreenPage::RedPowerUp;
				}

				else if (m_ThreePlayersButton->isMouseOver())
				{
					m_NumberOfPlayers = 3;
					m_StartScreenPage = StartScreenPage::RedPowerUp;
				}

				break;

			case StartScreenPage::RedPowerUp:
				if (m_NextButton->isMouseOver())
				{
					m_StartScreenPage = StartScreenPage::BluePowerUp;

					m_SpeedPowerupChosen = false;
					m_AccuracyPowerupChosen = false;
					m_DamagePowerupChosen = false;
					m_CooldownPowerupChosen = false;

					m_PowerupChoosingColour = SDL_Color { 0, 0, 255, 255 };
				}

				break;

			case StartScreenPage::BluePowerUp:
				if (m_NextButton->isMouseOver())
				{
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
					m_GameState = GameState::Gameplay;
					initGameplay();
				}

				break;

			default:
				break;
			}

			if (m_StartScreenPage == StartScreenPage::RedPowerUp ||
				m_StartScreenPage == StartScreenPage::BluePowerUp ||
				m_StartScreenPage == StartScreenPage::GreyPowerUp)
			{
				if (m_SpeedPowerupButton->isMouseOver())
				{
					m_SpeedPowerupChosen = !m_SpeedPowerupChosen;
				}

				else if (m_AccuracyPowerupButton->isMouseOver())
				{
					m_AccuracyPowerupChosen = !m_AccuracyPowerupChosen;
				}

				else if (m_DamagePowerupButton->isMouseOver())
				{
					m_DamagePowerupChosen = !m_DamagePowerupChosen;
				}

				else if (m_CooldownPowerupButton->isMouseOver())
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
		m_TwoPlayersButton->update();
		m_ThreePlayersButton->update();

		break;

	case StartScreenPage::RedPowerUp:
	case StartScreenPage::BluePowerUp:
	case StartScreenPage::GreyPowerUp:
		m_SpeedPowerupButton->update();
		m_AccuracyPowerupButton->update();
		m_DamagePowerupButton->update();
		m_CooldownPowerupButton->update();
		m_NextButton->update();

		break;

	default:
		break;
	}
}

void Game::drawStartScreen()
{
	SDL_RenderClear(m_Renderer);

	// Draws background
	SDL_RenderCopy(m_Renderer, m_SpaceBackgroundTexture, nullptr, &m_SpaceBackgroundRect);

	switch (m_StartScreenPage)
	{
	case StartScreenPage::NumberOfPlayersChoice:
		m_ReductionText.draw(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 5 / 20);
		m_TwoPlayersButton->draw(SCREEN_WIDTH * 6 / 20, SCREEN_HEIGHT * 11 / 20);
		m_ThreePlayersButton->draw(SCREEN_WIDTH * 14 / 20, SCREEN_HEIGHT * 11 / 20);

		break;

	case StartScreenPage::RedPowerUp:
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
