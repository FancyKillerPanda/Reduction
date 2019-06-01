#pragma once

#include "Text.h"


class Button
{
private:
	// Renderer for drawing
	SDL_Renderer* m_Renderer;

	// Text that represents the button
	Text m_Text;

	// Colours for the text
	SDL_Color m_DefaultTextColour = { 255, 255, 255, 255 };
	SDL_Color m_HighlightedTextColour = { 0, 255, 0, 255 };

	bool m_CurrentlyHighlighting = false;

public:
	Button(SDL_Renderer* renderer, const char* text);

	// Updates the button
	void update();

	// Draws the button
	void draw(unsigned int centerX, unsigned int centerY);

	// Returns true when clicked
	bool isMouseOver();

	// Returns the text object so it can be modified
	Text& getText() { return m_Text; }

	// Sets the default text colour
	void setTextColour(SDL_Color colour) { m_DefaultTextColour = colour; }
	// Sets the highlight text colour
	void setHighlightedTextColour(SDL_Color colour) { m_HighlightedTextColour = colour; }
};
