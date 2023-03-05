#pragma once

#include "Game.h"
#include "SDL.h"

class Window
{
public:
	Window();
	~Window();

	void run();

	static SDL_Renderer* m_Renderer;

private:
	bool init();

	bool m_Closed = false;
	SDL_Window* m_Window = nullptr;
	Game* m_Game = nullptr;
};