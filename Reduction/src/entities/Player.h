#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


class Player
{
private:
	SDL_Texture* m_Texture = nullptr;
	SDL_Rect m_Rect;

	SDL_Renderer* m_Renderer;

	// Current movement attributes
	double m_Direction = 45.0;

public:
	Player(SDL_Renderer* renderer);

	void draw();
};
