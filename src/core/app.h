#pragma once

struct SDL_Window;
struct SDL_Renderer;

class App
{
public:
	void run();
private:
	void init();
	void update();
	void render();
	void quit();

	void initSDL();
	void initImgui();

	SDL_Window* _window;
	SDL_Renderer* _renderer;
};