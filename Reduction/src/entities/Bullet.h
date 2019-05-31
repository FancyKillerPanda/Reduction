#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


class Bullet
{
private:
	SDL_Texture* m_Texture = nullptr;
	SDL_Rect m_Rect;

	SDL_Renderer* m_Renderer;

	// Current movement attributes
	double m_Direction = 0.0;
	double m_PosX = 0.0, m_PosY = 0.0;

public:
	Bullet(SDL_Renderer* renderer, double direction, double posX, double posY);

	bool update(double dt);
	void draw();

	SDL_Rect& getRect() { return m_Rect; }
};
