#include "Bullet.h"

#include "utils/MathUtils.h"
#include "utils/Settings.h"
#include "utils/Log.h"


Bullet::Bullet(SDL_Renderer* renderer, double direction, double posX, double posY)
	: m_Renderer(renderer), m_Direction(direction), m_PosX(posX), m_PosY(posY)
{
	m_Texture = IMG_LoadTexture(m_Renderer, "res/Bullet.png");

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


void Bullet::update(double dt)
{
	m_PosX += std::cos(toRadians(m_Direction)) * BULLET_SPEED * dt;
	m_PosY += std::sin(toRadians(m_Direction)) * BULLET_SPEED * dt;

	m_Rect.x = m_PosX;
	m_Rect.y = m_PosY;
}

void Bullet::draw()
{
	SDL_RenderCopy(m_Renderer, m_Texture, nullptr, &m_Rect);
}
