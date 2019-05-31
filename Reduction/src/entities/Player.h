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
	double m_Direction = 0.0;
	double m_RotationSpeed = 0.0;
	double m_PosX = 0.0, m_PosY = 0.0;
	double m_Velocity = 0.0;
	double m_Acceleration = 0.0;
	double m_Drag = 0.0;

public:
	Player(SDL_Renderer* renderer);

	void update(double dt);
	void draw();

	void setRotationSpeed(double value) { m_RotationSpeed = value; }
	void setAcceleration(double value) { m_Acceleration = value; }
};
