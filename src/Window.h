#pragma once

#include "SDL.h"
#include <string>
#include "Game.h"

class Window
{
public:
	Window(const std::string& title, int width, int height);
	~Window();

	void pollEvents();
	inline bool isClosed() const { return m_Closed; }

	static SDL_Renderer* m_Renderer;

	static int m_Width;
	static int m_Height;
private:
	std::string m_Title;

	bool init();
	bool m_Closed = false;

	SDL_Window* m_Window;
	Game* game;
};