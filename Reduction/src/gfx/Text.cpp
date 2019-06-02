#include "Text.h"

#include "utils/Log.h"


Text::Text(const char* fontPath, std::string text, unsigned int size, SDL_Color colour, SDL_Renderer* renderer)
{
	load(fontPath, text, size, colour, renderer);
}

Text::~Text()
{
	// Destroys the texture
	SDL_DestroyTexture(m_TextTexture);
	m_TextTexture = nullptr;

	// Closes the font
	TTF_CloseFont(m_Font);
	m_Font = nullptr;
}

void Text::load(const char* fontPath, std::string text, unsigned int size, SDL_Color colour, SDL_Renderer* renderer)
{
	// Sets attributes on load
	m_FontPath = fontPath;
	m_Text = text;
	m_Size = size;
	m_Colour = colour;
	m_Renderer = renderer;

	// Loads the font
	m_Font = TTF_OpenFont(m_FontPath, size);

	// Error checking for font
	if (m_Font == nullptr)
	{
		error("Could not load font (filepath: ", m_FontPath, ").\nSDLError: ", SDL_GetError());
		return;
	}

	updateTexture();

	m_Loaded = true;
}


void Text::updateTexture()
{
	// Destroys the last texture
	SDL_DestroyTexture(m_TextTexture);
	m_TextTexture = nullptr;

	// Creates a surface for the font
	SDL_Surface* textSurface = TTF_RenderUTF8_Solid(m_Font, m_Text.c_str(), m_Colour);

	// Error checking for text surface
	if (textSurface == nullptr)
	{
		error("Could not create text surface (filepath: ", m_FontPath, ").\nSDLError: ", SDL_GetError());
		return;
	}

	// Creates a texture from the surface
	m_TextTexture = SDL_CreateTextureFromSurface(m_Renderer, textSurface);

	if (m_TextTexture == nullptr)
	{
		error("Could not create texture from surface for font (filepath: ", m_FontPath, ").\nSDLError: ", SDL_GetError());
		return;
	}

	// Frees the temporary surface
	SDL_FreeSurface(textSurface);

	// Gets the dimensions of the texture
	SDL_QueryTexture(m_TextTexture, nullptr, nullptr, &m_TextRect.w, &m_TextRect.h);
}

bool Text::rectCollides(int x, int y)
{
	SDL_Rect mouseRect = { x, y, 1, 1 };
	return SDL_HasIntersection(&mouseRect, &m_TextRect);
}

void Text::setText(std::string text, bool update)
{
	if (!m_Loaded)
	{
		warn("Tried to set text before loading.");
		return;
	}

	m_Text = text;

	if (update)
	{
		updateTexture();
	}
}

void Text::setColour(const SDL_Color& colour, bool update)
{
	if (!m_Loaded)
	{
		warn("Tried to set text colour before loading.");
		return;
	}

	m_Colour = colour;

	if (update)
	{
		updateTexture();
	}
}

void Text::setSize(unsigned int size, bool update)
{
	if (!m_Loaded)
	{
		warn("Tried to set text size before loading.");
		return;
	}

	m_Size = size;

	// Closes the old font
	TTF_CloseFont(m_Font);

	// Loads the font
	m_Font = TTF_OpenFont(m_FontPath, size);

	// Error checking for font
	if (m_Font == nullptr)
	{
		error("Could not load font (filepath: ", m_FontPath, ").\nSDLError: ", SDL_GetError());
		return;
	}

	if (update)
	{
		updateTexture();
	}
}

void Text::setStyle(int style, bool update)
{
	TTF_SetFontStyle(m_Font, style);

	if (update)
	{
		updateTexture();
	}
}

void Text::draw(unsigned int x, unsigned int y)
{
	if (!m_Loaded)
	{
		error("Tried to draw text before loading.");
		return;
	}

	// Sets the text position (aligned center)
	m_TextRect.x = x - (m_TextRect.w / 2);
	m_TextRect.y = y - (m_TextRect.h / 2);

	SDL_RenderCopy(m_Renderer, m_TextTexture, nullptr, &m_TextRect);
}
