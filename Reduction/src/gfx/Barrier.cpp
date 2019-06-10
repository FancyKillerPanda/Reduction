#include "Barrier.h"

#include "utils/Log.h"


Barrier::Barrier(SDL_Renderer* renderer, int xPos, int yPos, double rotation)
	: m_Renderer(renderer), m_Rotation(rotation)
{
	m_Texture = IMG_LoadTexture(m_Renderer, "res/txrs/Barrier.png");

	if (!m_Texture)
	{
		error("Could not load Barrier texture.\nSDL_Error: ", SDL_GetError());
		return;
	}

	if (SDL_QueryTexture(m_Texture, nullptr, nullptr, &m_Rect.w, &m_Rect.h) != 0)
	{
		error("Barrier texture is invalid.\nSDL_Error: ", SDL_GetError());
		return;
	}

	m_Rect.w *= 0.7;
	m_Rect.h *= 0.7;
	m_Rect.x = xPos - m_Rect.w / 2;
	m_Rect.y = yPos - m_Rect.h / 2;
}


void Barrier::draw()
{
	SDL_RenderCopyEx(m_Renderer, m_Texture, nullptr, &m_Rect, m_Rotation, nullptr, SDL_FLIP_NONE);
}
