#include <SDL3/SDL.h>
#include "log.h"

int main(int argc, char* argv[])
{
	if (!SDL_Init(SDL_INIT_VIDEO)) 
	{
		D_ASSERT(false, "Failed to init SDL: %s", SDL_GetError());
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Test", 960, 540, SDL_WINDOW_RESIZABLE);
	if (!window) 
	{
		D_ASSERT(false, "Failed to create window: %s", SDL_GetError());
		return 1;
	}

	SDL_Delay(5000);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}