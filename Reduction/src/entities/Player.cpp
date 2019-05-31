#include "Player.h"

#include "utils/Log.h"
#include "utils/Settings.h"
#include "utils/MathUtils.h"


Player::Player(SDL_Renderer* renderer)
	: m_Renderer(renderer)
{
	m_Texture = IMG_LoadTexture(m_Renderer, "res/Blue Spaceship.png");

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
}


void Player::update(double dt)
{
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
}
