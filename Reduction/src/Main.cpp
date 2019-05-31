#include "Game.h"


int main(int argc, char* argv[])
{
	Game* reduction = new Game();
	reduction->run();
	delete reduction;

	return 0;
}
