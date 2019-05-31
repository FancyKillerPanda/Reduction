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
	m_Player = new Player(m_Renderer);

	m_FrameTimer.reset();
}

Game::~Game()
{
	// Deletes players
	delete m_Player;

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
				m_Player->setRotationSpeed(-PLAYER_ROTATION_SPEED);
				break;

			case SDLK_RIGHT:
				m_Player->setRotationSpeed(PLAYER_ROTATION_SPEED);
				break;

			case SDLK_UP:
				m_Player->setAcceleration(PLAYER_ACCELERATION);
				break;

			case SDLK_DOWN:
				m_Player->setAcceleration(-PLAYER_ACCELERATION * 2 / 3);
				break;

			case SDLK_m:
				m_Bullets.push_back(m_Player->spawnBullet());

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
				m_Player->setRotationSpeed(0.0);
				break;

			case SDLK_UP:
			case SDLK_DOWN:
				m_Player->setAcceleration(0.0);
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
	m_Player->update(dt);

	// Updates bullets
	for (Bullet* bullet : m_Bullets)
	{
		bullet->update(dt);
	}
}

void Game::draw()
{
	SDL_RenderClear(m_Renderer);

	// Draws players
	m_Player->draw();

	// Draws bullets
	for (Bullet* bullet : m_Bullets)
	{
		bullet->draw();
	}

	SDL_RenderPresent(m_Renderer);
}
