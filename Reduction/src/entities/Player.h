#pragma once

#include <vector>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "Bullet.h"
#include "utils/Timer.h"
#include "utils/Settings.h"


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

	PlayerColour m_Colour;

	// Current movement attributes
	double m_Direction = 0.0;
	double m_RotationSpeed = 0.0;
	double m_PosX = 0.0, m_PosY = 0.0;
	double m_Velocity = 0.0;
	double m_Acceleration = 0.0;
	double m_Drag = 0.0;

	// Keeps track of each bullet
	std::vector<Bullet*> m_Bullets;

	// Bullet cooldown time
	Timer m_BulletCooldownTimer;

	// The amount of life the player has left
	int m_LifeLeft = PLAYER_STARTING_LIFE;
	SDL_Rect m_LifeBarRect;

public:
	Player(SDL_Renderer* renderer, PlayerColour colour, double posX, double posY, double direction);
	~Player();

	void update(double dt);
	void draw();

	void spawnBullet();
	void updateBullets(double dt);
	void drawBullets();
	void takeHit();
	void updateLifeBar();

	void setRotationSpeed(double value) { m_RotationSpeed = value; }
	void setAcceleration(double value) { m_Acceleration = value; }
	void setCenter(double x, double y) { m_PosX = x - m_Rect.w / 2; m_PosY = y - m_Rect.h / 2; }

	SDL_Rect& getRect() { return m_Rect; }
	std::vector<Bullet*>& getBullets() { return m_Bullets; }
	int getLifeLeft() { return m_LifeLeft; }
};
