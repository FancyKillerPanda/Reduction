#include <SDL/SDL.h>

#include "utils/Log.h"


constexpr int SCREEN_WIDTH = 960;
constexpr int SCREEN_HEIGHT = 540;


int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		error("Could not initialise SDL.");
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("Reduction", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!window)
	{
		error("Could not create window.");
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!renderer)
	{
		error("Could not create renderer.");
		return -1;
	}

	bool running = true;
	SDL_Event event;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			}
		}

		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	return 0;
}
