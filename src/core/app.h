#pragma once

#include <stdint.h>

struct SDL_Window;
struct SDL_Renderer;

class App
{
public:
	void run();
private:
	void init();
	void update();
	void render(float alpha);
	void quit();

	void initSDL();
	void initImgui();

	SDL_Window* _window;
};