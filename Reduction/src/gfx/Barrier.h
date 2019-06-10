#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


class Barrier
{
	SDL_Texture* m_Texture;
	SDL_Rect m_Rect;
	SDL_Renderer* m_Renderer;

	// Rotation of the barrier
	double m_Rotation = 0.0;

public:
	Barrier(SDL_Renderer* renderer, int xPos, int yPos, double rotation);

	void draw();
};

