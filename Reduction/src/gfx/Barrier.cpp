#include "Barrier.h"

#include "utils/Log.h"
#include "utils/Settings.h"


Barrier::Barrier(SDL_Renderer* renderer, int xPos, int yPos)
	: m_Renderer(renderer)
{
	m_VerticalRect.w = BARRIER_WIDTH;
	m_VerticalRect.h = BARRIER_LENGTH;
	m_HorizontalRect.w = BARRIER_LENGTH;
	m_HorizontalRect.h = BARRIER_WIDTH;

	if (xPos < SCREEN_WIDTH / 2)
	{
		m_VerticalRect.x = xPos - m_VerticalRect.w;
		m_HorizontalRect.x = xPos - m_HorizontalRect.w;
	}

	else
	{
		m_VerticalRect.x = xPos;
		m_HorizontalRect.x = xPos;
	}

	if (yPos < SCREEN_HEIGHT / 2)
	{
		m_VerticalRect.y = yPos - m_VerticalRect.h;
		m_HorizontalRect.y = yPos - m_HorizontalRect.h;
	}

	else
	{
		m_VerticalRect.y = yPos;
		m_HorizontalRect.y = yPos;
	}
}


void Barrier::draw()
{
	SDL_SetRenderDrawColor(m_Renderer, 0, 191, 0, 255);
	SDL_RenderFillRect(m_Renderer, &m_VerticalRect);
	SDL_RenderFillRect(m_Renderer, &m_HorizontalRect);
}
