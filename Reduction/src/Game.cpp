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
}

Game::~Game()
{
	// Deletes players
	delete m_Player;
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
		}
	}
}

void Game::update()
{
}

void Game::draw()
{
	SDL_RenderClear(m_Renderer);
	m_Player->draw();
	SDL_RenderPresent(m_Renderer);
}
