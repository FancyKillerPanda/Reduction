#pragma once

#include <vector>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "gfx/Barrier.h"


class Bullet
{
private:
	SDL_Texture* m_Texture = nullptr;
	SDL_Rect m_Rect;

	SDL_Renderer* m_Renderer;

	// Current movement attributes
	double m_Direction = 0.0;
	double m_PosX = 0.0, m_PosY = 0.0;

	// Does extra damage when power-up used
	bool m_DoesExtraDamage = false;

public:
	Bullet(SDL_Renderer* renderer, double direction, double posX, double posY, bool doesExtraDamage = false);

	bool update(double dt, const std::vector<Barrier>& barriers);
	void draw();

	SDL_Rect& getRect() { return m_Rect; }
	double getDirection() { return m_Direction; }
	bool doesExtraDamage() { return m_DoesExtraDamage; }
};
