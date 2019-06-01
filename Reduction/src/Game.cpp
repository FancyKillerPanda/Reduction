#include "Game.h"

#include "utils/Settings.h"
#include "utils/Log.h"


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
	SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, 255);

	initGameplay();
	m_FrameTimer.reset();
}

Game::~Game()
{
	// Deletes players
	for (Player* player : m_Players)
	{
		delete player;
	}
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

			case SDLK_m:
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
		}
	}
}

void Game::updateGameplay()
{
	// Number of seconds since last frame
	double dt = m_FrameTimer.getElapsed() / 1000;
	m_FrameTimer.reset();

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
		}
	}
}

void Game::updateStartScreen()
{
}

void Game::drawStartScreen()
{
	SDL_RenderClear(m_Renderer);

	SDL_RenderPresent(m_Renderer);
}

