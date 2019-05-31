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

	// Initialises the players
	m_Players.push_back(new Player(m_Renderer, PlayerColour::Red));
	m_Players.push_back(new Player(m_Renderer, PlayerColour::Blue));

	m_FrameTimer.reset();
}

Game::~Game()
{
	// Deletes players
	for (Player* player : m_Players)
	{
		delete player;
	}

	// Destroys bullets
	for (Bullet* bullet : m_Bullets)
	{
		delete bullet;
	}
}


void Game::run()
{
	while (m_Running)
	{
		handleEvents();
		update();
		draw();
	}
}

void Game::handleEvents()
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
				m_Bullets.push_back(m_Players[0]->spawnBullet());

				if (m_Bullets.back() == nullptr)
				{
					m_Bullets.pop_back();
				}

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
				m_Bullets.push_back(m_Players[1]->spawnBullet());

				if (m_Bullets.back() == nullptr)
				{
					m_Bullets.pop_back();
				}

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

void Game::update()
{
	// Number of seconds since last frame
	double dt = m_FrameTimer.getElapsed() / 1000;
	m_FrameTimer.reset();

	// Updates players
	for (Player* player : m_Players)
	{
		player->update(dt);
	}

	// Updates bullets
	for (unsigned int i = 0; i < m_Bullets.size(); i++)
	{
		Bullet* bullet = m_Bullets[i];

		if (!bullet->update(dt))
		{
			delete bullet;

			// Removes the bullet from the vector
			m_Bullets.erase(m_Bullets.begin() + i);
			i -= 1;
		}
	}
}

void Game::draw()
{
	SDL_RenderClear(m_Renderer);

	// Draws players
	for (Player* player : m_Players)
	{
		player->draw();
	}

	// Draws bullets
	for (Bullet* bullet : m_Bullets)
	{
		bullet->draw();
	}

	SDL_RenderPresent(m_Renderer);
}
