#include "Player.h"

#include "utils/Log.h"
#include "utils/Settings.h"
#include "utils/MathUtils.h"


Player::Player(SDL_Renderer* renderer, PlayerColour colour, double posX, double posY, double direction)
	: m_Renderer(renderer), m_Colour(colour)
{
	const char* file;

	switch (colour)
	{
	case PlayerColour::Red:
		file = "res/Red Spaceship.png";
		break;

	case PlayerColour::Blue:
		file = "res/Blue Spaceship.png";
		break;

	case PlayerColour::Grey:
		file = "res/Grey Spaceship.png";
		break;

	default:
		error("Unhandled player colour case.");
		return;
	}

	// Loads the texture
	m_Texture = IMG_LoadTexture(m_Renderer, file);

	if (!m_Texture)
	{
		error("Could not load Player texture.\nSDL_Error: ", SDL_GetError());
		return;
	}

	if (SDL_QueryTexture(m_Texture, nullptr, nullptr, &m_Rect.w, &m_Rect.h) != 0)
	{
		error("Player texture is invalid.\nSDL_Error: ", SDL_GetError());
		return;
	}

	// Sets the position and angle
	setCenter(posX, posY);
	m_Direction = direction;

	// Sets the width and height of the life bar
	m_LifeBarRect.w = LIFE_BAR_FULL_WIDTH;
	m_LifeBarRect.h = LIFE_BAR_HEIGHT;

	// Sets the position of the life bar
	switch (colour)
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

Player::~Player()
{
	// Destroys bullets
	for (Bullet* bullet : m_Bullets)
	{
		delete bullet;
	}
}


void Player::update(double dt)
{
	if (m_Velocity < 0.0)
	{
		m_Velocity = 0.0;
	}

	m_Direction += std::fmod(m_RotationSpeed * dt, 360.0);
	m_Drag = PLAYER_DRAG + m_Velocity / 2;

	if (m_Velocity > 0)
	{
		m_Velocity -= m_Drag * dt;

		if (m_Velocity < 0)
		{
			m_Velocity = 0;
		}
	}

	if (m_Velocity < MAX_PLAYER_SPEED)
	{
		m_Velocity += m_Acceleration * dt;
	}

	m_PosX += std::cos(toRadians(m_Direction)) * m_Velocity * dt;
	m_PosY += std::sin(toRadians(m_Direction)) * m_Velocity * dt;

	m_Rect.x = (int) m_PosX;
	m_Rect.y = (int) m_PosY;
}

void Player::draw()
{
	SDL_RenderCopyEx(m_Renderer, m_Texture, nullptr, &m_Rect, m_Direction, nullptr, SDL_FLIP_NONE);

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

	// Draws the life bar
	SDL_RenderFillRect(m_Renderer, &m_LifeBarRect);

	// Returns the drawing colour back to what it was
	SDL_SetRenderDrawColor(m_Renderer, r, g, b, a);
}

void Player::spawnBullet()
{
	if (m_BulletCooldownTimer.getElapsed() >= BULLET_COOLDOWN)
	{
		m_BulletCooldownTimer.reset();
		m_Bullets.push_back(new Bullet(m_Renderer, m_Direction, m_Rect.x + m_Rect.w / 2, m_Rect.y + m_Rect.h / 2));
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

void Player::takeHit()
{
	m_LifeLeft -= PLAYER_HIT_DAMAGE;

	if (m_LifeLeft < 0)
	{
		m_LifeLeft = 0;
	}
}
