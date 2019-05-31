#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "Bullet.h"
#include "utils/Timer.h"


enum class PlayerColour
{
	Red,
	Blue,
	Grey,
};


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

	// Bullet cooldown time
	Timer m_BulletCooldownTimer;

public:
	Player(SDL_Renderer* renderer, PlayerColour colour);

	void update(double dt);
	void draw();

	Bullet* spawnBullet();

	void setRotationSpeed(double value) { m_RotationSpeed = value; }
	void setAcceleration(double value) { m_Acceleration = value; }
};
