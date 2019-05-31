#include "Player.h"

#include "utils/Log.h"


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


void Player::draw()
{
	SDL_RenderCopy(m_Renderer, m_Texture, nullptr, &m_Rect);
}
