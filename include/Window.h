#pragma once

#include "Game.h"
#include "SDL.h"

class Window
{
public:
	Window();
	~Window();

	void pollEvents();
	inline bool isClosed() const { return m_Closed; }
	static SDL_Renderer* m_Renderer;

private:
	bool init();
	bool m_Closed = false;
	SDL_Window* m_Window;
	Game* game;
};