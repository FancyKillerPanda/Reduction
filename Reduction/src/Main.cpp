#include <SDL/SDL.h>

#include "utils/Log.h"


int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		error("Could not initialise SDL.");
		return -1;
	}

	return 0;
}
