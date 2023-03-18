#include "Window.h"
#include "Piece.h"

#include <iostream>
#include <SDL2/SDL_image.h>

SDL_Renderer* Window::m_Renderer = nullptr;

Window::Window()
{
	m_Closed = !init();
}

Window::~Window()
{
	SDL_DestroyRenderer(m_Renderer);
	SDL_DestroyWindow(m_Window);
	Piece::destroyImages();
	IMG_Quit();
	SDL_Quit();
}

bool Window::init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "Failed to initalize SDL.\n";
		return false;
	}

	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	int initted = IMG_Init(flags);

	if ((initted & flags) != flags)
	{
		std::cerr << "IMG_Init: Failed to init required jpg and png support!\n";
		std::cerr << "IMG_Init: " << IMG_GetError() << "\n";
		return false;
	}

	m_Window = SDL_CreateWindow(
		"Chess",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640, 640,
		0
	);

	if (!m_Window)
	{
		std::cerr << "Failed to create window.\n";
		return false;
	}

	m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_PRESENTVSYNC);

	if (!m_Renderer)
	{
		std::cerr << "Failed to create renderer.\n";
		return false;
	}

	Piece::init();

	// The game has to be initialized after the pieces
	m_Game.init();

	return true;
}

void Window::run()
{
	SDL_Event event;

	while (!m_Closed)
	{
		// INPUT/UPDATE
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					m_Closed = true;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							m_Closed = true;
							break;
						case SDLK_r:
							m_Game.printCurrentMoveCount(4);
							break;
						case SDLK_t:
							m_Game.printAllTestsMoveCount(4);
							break;
						case SDLK_f:
							m_Game.inputFEN();
						case SDLK_e:
							m_Game.printNextMoveCount();
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					m_Game.handleMouseButton(event.button);
					break;
				default:
					break;
			}
		}

		// RENDER
		m_Game.refreshAllSquares();
		SDL_RenderPresent(m_Renderer);
	}
}