#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


class Barrier
{
	SDL_Rect m_VerticalRect;
	SDL_Rect m_HorizontalRect;
	SDL_Renderer* m_Renderer;

	// Rotation of the barrier
	double m_Rotation = 0.0;

public:
	Barrier(SDL_Renderer* renderer, int xPos, int yPos);

	void draw();

	const SDL_Rect& getHorizontalRect() const { return m_HorizontalRect; }
	const SDL_Rect& getVerticalRect() const { return m_VerticalRect; }
};

