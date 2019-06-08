#include "Button.h"

#include "utils/Log.h"


Button::Button(SDL_Renderer* renderer, const char* text)
	: m_Renderer(renderer)
{
	if (std::strcmp(text, "") == 0)
	{
		error("Cannot make button with empty text.");
	}

	m_Text.load("res/fonts/BM Space.TTF", text, 28, m_DefaultTextColour, m_Renderer);
}


void Button::update()
{
	if (isMouseOver())
	{
		if (!m_CurrentlyHighlighting)
		{
			m_Text.setStyle(TTF_STYLE_BOLD, false);
			m_Text.setColour(m_HighlightedTextColour);

			m_CurrentlyHighlighting = true;
		}
	}

	else
	{
		if (m_CurrentlyHighlighting)
		{
			m_Text.setStyle(TTF_STYLE_NORMAL, false);
			m_Text.setColour(m_DefaultTextColour);

			m_CurrentlyHighlighting = false;
		}
	}
}

void Button::draw(unsigned int centerX, unsigned int centerY)
{
	m_Text.draw(centerX, centerY);
}

bool Button::isMouseOver()
{
	// Gets the mouse position
	int mouseX;
	int mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	return m_Text.rectCollides(mouseX, mouseY);
}
