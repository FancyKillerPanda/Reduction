#include "Player.h"

#include "utils/Log.h"
#include "utils/Settings.h"
#include "utils/MathUtils.h"
#include "utils/Random.h"


Player::Player(SDL_Renderer* renderer, PlayerColour colour, double posX, double posY, double direction)
	: m_Renderer(renderer), m_Colour(colour)
{
	std::string file;

	switch (colour)
	{
	case PlayerColour::Red:
		file.append("res/Red Spaceship");
		break;

	case PlayerColour::Blue:
		file.append("res/Blue Spaceship");
		break;

	case PlayerColour::Grey:
		file.append("res/Grey Spaceship");
		break;

	default:
		error("Unhandled player colour case.");
		return;
	}

	// Loads the texture
	std::string filename = file + ".png";
	m_NoFlameTexture = IMG_LoadTexture(m_Renderer, filename.c_str());
	filename = file + " - Small Flame.png";
	m_SmallFlameTexture = IMG_LoadTexture(m_Renderer, filename.c_str());
	filename = file + " - Medium Flame.png";
	m_MediumFlameTexture = IMG_LoadTexture(m_Renderer, filename.c_str());
	filename = file + " - Large Flame.png";
	m_LargeFlameTexture = IMG_LoadTexture(m_Renderer, filename.c_str());

	if (!(m_NoFlameTexture && m_SmallFlameTexture && m_MediumFlameTexture && m_LargeFlameTexture))
	{
		error("Could not load Player texture.\nSDL_Error: ", SDL_GetError());
		return;
	}

	if (SDL_QueryTexture(m_NoFlameTexture, nullptr, nullptr, &m_Rect.w, &m_Rect.h) != 0)
	{
		error("Player texture is invalid.\nSDL_Error: ", SDL_GetError());
		return;
	}

	// Sets the active texture to no-flame
	m_ActiveTexture = m_NoFlameTexture;

	// Sets the position and angle
	setCenter(posX, posY);
	m_Direction = direction;

	updateLifeBar();

	// Sets the dimensions and position for the life bar outline
	m_LifeBarOutlineRect.w = LIFE_BAR_FULL_WIDTH;
	m_LifeBarOutlineRect.h = LIFE_BAR_HEIGHT;

	switch (m_Colour)
	{
	case PlayerColour::Red:
		m_LifeBarOutlineRect.x = 30;
		m_LifeBarOutlineRect.y = 30;

		break;

	case PlayerColour::Blue:
		m_LifeBarOutlineRect.x = SCREEN_WIDTH - 30 - m_LifeBarOutlineRect.w;
		m_LifeBarOutlineRect.y = 30;

		break;

	case PlayerColour::Grey:
		m_LifeBarOutlineRect.x = 30;
		m_LifeBarOutlineRect.y = SCREEN_HEIGHT - 30 - m_LifeBarOutlineRect.h;

		break;

	default:
		break;
	}
}

Player::~Player()
{
	// Destroys bullets
	for (Bullet* bullet : m_Bullets)
	{
		delete bullet;
	}
}


void Player::update(double dt, double wallScale)
{
	if (m_Velocity < 0.0)
	{
		m_Velocity = 0.0;
	}

	m_Direction += std::fmod(m_RotationSpeed * dt, 360.0);
	m_Drag = PLAYER_DRAG + m_Velocity / 2 - m_DragReduction;

	if (m_Velocity > 0)
	{
		m_Velocity -= m_Drag * dt;

		if (m_Velocity < 0)
		{
			m_Velocity = 0;
		}
	}

	if (m_Velocity < MAX_PLAYER_SPEED + m_ExtraSpeed)
	{
		m_Velocity += m_Acceleration * dt;
	}

	m_PosX += std::cos(toRadians(m_Direction)) * m_Velocity * dt;
	m_PosY += std::sin(toRadians(m_Direction)) * m_Velocity * dt;

	m_Rect.x = (int) m_PosX;
	m_Rect.y = (int) m_PosY;

	// Calculates distance from center
	double xDistanceFromCenter = (m_PosX + m_Rect.w / 2) - SCREEN_WIDTH / 2;
	double yDistanceFromCenter = (m_PosY + m_Rect.h / 2) - SCREEN_HEIGHT / 2;
	double distanceFromCenterSquared = (xDistanceFromCenter * xDistanceFromCenter) + (yDistanceFromCenter * yDistanceFromCenter);

	if (distanceFromCenterSquared > (SCREEN_HEIGHT * wallScale / 2) * (SCREEN_HEIGHT * wallScale / 2))
	{
		m_LifeLeft -= (int) (distanceFromCenterSquared - (SCREEN_HEIGHT * wallScale / 2) * (SCREEN_HEIGHT * wallScale / 2)) / 20000 + 1;

		if (m_LifeLeft < 0)
		{
			m_LifeLeft = 0;
		}

		updateLifeBar();
	}

	// Updates texture
	if (m_Velocity > MAX_PLAYER_SPEED * 2 / 4)
	{
		m_ActiveTexture = m_LargeFlameTexture;
	}

	else if (m_Velocity > MAX_PLAYER_SPEED * 1 / 4)
	{
		m_ActiveTexture = m_MediumFlameTexture;
	}

	else if (m_Velocity > 0)
	{
		m_ActiveTexture = m_SmallFlameTexture;
	}

	if (m_Acceleration <= 0)
	{
		m_ActiveTexture = m_NoFlameTexture;
	}
}

void Player::draw()
{
	SDL_RenderCopyEx(m_Renderer, m_ActiveTexture, nullptr, &m_Rect, m_Direction, nullptr, SDL_FLIP_NONE);

	// Saves the current render colour
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
	SDL_GetRenderDrawColor(m_Renderer, &r, &g, &b, &a);

	// Sets the drawing colour
	switch (m_Colour)
	{
	case PlayerColour::Red:
		SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);
		break;

	case PlayerColour::Blue:
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 255, 255);
		break;

	case PlayerColour::Grey:
		SDL_SetRenderDrawColor(m_Renderer, 127, 127, 127, 255);
		break;

	default:
		break;
	}

	// Draws the life bar and outline
	SDL_RenderDrawRect(m_Renderer, &m_LifeBarOutlineRect);
	SDL_RenderFillRect(m_Renderer, &m_LifeBarRect);

	// Returns the drawing colour back to what it was
	SDL_SetRenderDrawColor(m_Renderer, r, g, b, a);
}

void Player::spawnBullet()
{
	if (m_BulletCooldownTimer.getElapsed() >= BULLET_COOLDOWN - m_BulletCooldownReduction)
	{
		m_BulletCooldownTimer.reset();

		double directionOffset = Random::randdouble(-BULLET_DIRECTION_OFFSET, BULLET_DIRECTION_OFFSET);

		if (m_DamagePowerup)
		{
			m_Bullets.push_back(new Bullet(m_Renderer, m_Direction + directionOffset, m_Rect.x + m_Rect.w / 2, m_Rect.y + m_Rect.h / 2, true));
		}

		else
		{
			m_Bullets.push_back(new Bullet(m_Renderer, m_Direction + directionOffset, m_Rect.x + m_Rect.w / 2, m_Rect.y + m_Rect.h / 2));
		}
	}
}

void Player::updateBullets(double dt)
{
	for (unsigned int i = 0; i < m_Bullets.size(); i++)
	{
		Bullet* bullet = m_Bullets[i];

		if (!bullet->update(dt))
		{
			delete bullet;

			// Removes the bullet from the vector
			m_Bullets.erase(m_Bullets.begin() + i);
			i -= 1;
		}
	}
}

void Player::drawBullets()
{
	for (Bullet* bullet : m_Bullets)
	{
		bullet->draw();
	}
}

void Player::takeHit(Bullet* bullet)
{
	if (bullet->doesExtraDamage())
	{
		m_LifeLeft -= (int) PLAYER_HIT_DAMAGE + BULLET_EXTRA_DAMAGE;
	}

	else
	{
		m_LifeLeft -= (int) PLAYER_HIT_DAMAGE;
	}

	if (m_LifeLeft < 0)
	{
		m_LifeLeft = 0;
	}

	updateLifeBar();
}

void Player::updateLifeBar()
{
	// Sets the width and height of the life bar
	m_LifeBarRect.w = (int) ((m_LifeLeft / PLAYER_STARTING_LIFE) * LIFE_BAR_FULL_WIDTH);
	m_LifeBarRect.h = (int) LIFE_BAR_HEIGHT;

	// Sets the position of the life bar
	switch (m_Colour)
	{
	case PlayerColour::Red:
		m_LifeBarRect.x = 30;
		m_LifeBarRect.y = 30;

		break;

	case PlayerColour::Blue:
		m_LifeBarRect.x = SCREEN_WIDTH - 30 - m_LifeBarRect.w;
		m_LifeBarRect.y = 30;
		break;

	case PlayerColour::Grey:
		m_LifeBarRect.x = 30;
		m_LifeBarRect.y = SCREEN_HEIGHT - 30 - m_LifeBarRect.h;

		break;

	default:
		break;
	}
}

void Player::setPowerups(bool speed, bool accuracy, bool damage, bool cooldown)
{
	m_SpeedPowerup = speed;
	m_AccuracyPowerup = accuracy;
	m_DamagePowerup = damage;
	m_CooldownPowerup = cooldown;

	if (m_SpeedPowerup)
	{
		m_ExtraSpeed = SPEED_POWERUP_BOOST;
		m_DragReduction = DRAG_REDUCTION;
		m_LifeLeft -= (int) SPEED_POWERUP_COST;
	}

	else if (m_DamagePowerup)
	{
		m_LifeLeft -= (int) BULLET_DAMAGE_POWERUP_COST;
	}

	else if (m_CooldownPowerup)
	{
		m_BulletCooldownReduction = BULLET_COOLDOWN_REDUCTION;
		m_LifeLeft -= (int) BULLET_POWERUP_COST;
	}

	updateLifeBar();
}
