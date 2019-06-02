#include "Bullet.h"

#include "utils/MathUtils.h"
#include "utils/Settings.h"
#include "utils/Log.h"


Bullet::Bullet(SDL_Renderer* renderer, double direction, double posX, double posY, bool doesExtraDamage)
	: m_Renderer(renderer), m_Direction(direction), m_PosX(posX), m_PosY(posY), m_DoesExtraDamage(doesExtraDamage)
{
	m_Texture = IMG_LoadTexture(m_Renderer, "res/Bullet.png");

	if (!m_Texture)
	{
		error("Could not load Bullet texture.\nSDL_Error: ", SDL_GetError());
		return;
	}

	if (SDL_QueryTexture(m_Texture, nullptr, nullptr, &m_Rect.w, &m_Rect.h) != 0)
	{
		error("Bullet texture is invalid.\nSDL_Error: ", SDL_GetError());
		return;
	}
}


bool Bullet::update(double dt)
{
	m_PosX += std::cos(toRadians(m_Direction)) * BULLET_SPEED * dt;
	m_PosY += std::sin(toRadians(m_Direction)) * BULLET_SPEED * dt;

	m_Rect.x = (int) m_PosX;
	m_Rect.y = (int) m_PosY;

	if (m_PosX < 0.0 || m_PosX > SCREEN_WIDTH ||
		m_PosY < 0.0 || m_PosY > SCREEN_HEIGHT)
	{
		return false;
	}

	return true;
}

void Bullet::draw()
{
	SDL_RenderCopy(m_Renderer, m_Texture, nullptr, &m_Rect);
}
