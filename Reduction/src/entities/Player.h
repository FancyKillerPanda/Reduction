#pragma once

#include <vector>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#include "Bullet.h"
#include "gfx/Barrier.h"
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
	// Audio
	static Mix_Chunk* s_ShootSound;
	static Mix_Chunk* s_DeathSound;
	static Mix_Chunk* s_EngineSound;
	int m_EngineSoundChannel = -1;

	// Textures for different sized flames
	SDL_Texture* m_NoFlameTexture = nullptr;
	SDL_Texture* m_SmallFlameTexture = nullptr;
	SDL_Texture* m_MediumFlameTexture = nullptr;
	SDL_Texture* m_LargeFlameTexture = nullptr;
	SDL_Texture* m_ActiveTexture = nullptr;

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
	int m_LifeLeft = (int) PLAYER_STARTING_LIFE;
	SDL_Rect m_LifeBarRect;
	SDL_Rect m_LifeBarOutlineRect;

	// Powerups
	bool m_SpeedPowerup = false;
	double m_ExtraSpeed = 0.0;
	double m_DragReduction = 0.0;
	bool m_AccuracyPowerup = false;
	double m_BulletDirectionOffsetMax = BULLET_DIRECTION_OFFSET_MAX;
	bool m_DamagePowerup = false;
	bool m_CooldownPowerup = false;
	double m_BulletCooldownReduction = 0.0;

	// Whether the player is alive
	bool m_IsAlive = true;

	// Win count
	unsigned int m_Points = 0;

public:
	Player(SDL_Renderer* renderer, PlayerColour colour, double posX, double posY, double direction);
	~Player();

	void update(double dt, double wallScale, const std::vector<Barrier>& barriers);
	void draw();
	void reset(bool completeReset = false);

	void spawnBullet();
	void updateBullets(double dt, const std::vector<Barrier>& barriers);
	void drawBullets();
	void takeHit(Bullet* bullet);
	void updateLifeBar();

	static void initAudio();

	void setRotationSpeed(double value) { m_RotationSpeed = value; }
	void setAcceleration(double value) { m_Acceleration = value; }
	void setCenter(double x, double y) { m_PosX = x - m_Rect.w / 2; m_PosY = y - m_Rect.h / 2; }
	void setRotation(double value) { m_Direction = value; }
	void setVelocity(double value) { m_Velocity = value; }
	void setPowerups(bool speed, bool accuracy, bool damage, bool cooldown);
	void addPoint() { m_Points += 1; }
	void setLifeLeft(int value) { m_LifeLeft = value; }

	SDL_Rect& getRect() { return m_Rect; }
	std::vector<Bullet*>& getBullets() { return m_Bullets; }
	int getLifeLeft() { return m_LifeLeft; }
	bool isAlive() { return m_IsAlive; }
	unsigned int getPoints() { return m_Points; }
};
