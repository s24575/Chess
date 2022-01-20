#include "Window.h"
#include "BoardState.h"
#include <iostream>
#include <SDL2/SDL_image.h>
#include "Piece.h"

SDL_Renderer* Window::m_Renderer = nullptr;
int Window::m_Width = 0;
int Window::m_Height = 0;

Window::Window(const std::string &title, int width, int height)
{
	m_Width = width;
	m_Height = height;
	m_Closed = !init();
}

Window::~Window() {
	SDL_DestroyRenderer(m_Renderer);
	SDL_DestroyWindow(m_Window);
	Piece::destroyImages();
	IMG_Quit();
	SDL_Quit();
}

bool Window::init() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "Failed to initalize SDL.\n";
		return false;
	}

	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	int initted = IMG_Init(flags);

	if ((initted & flags) != flags) {
		printf("IMG_Init: Failed to init required jpg and png support!\n");
		printf("IMG_Init: %s\n", IMG_GetError());
		return false;
	}

	m_Window = SDL_CreateWindow(
		m_Title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_Width, m_Height,
		0
	);

	if (!m_Window) {
		std::cerr << "Failed to create window.\n";
		return false;
	}

	m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_PRESENTVSYNC);

	if (!m_Renderer) {
		std::cerr << "Failed to create renderer.\n";
		return false;
	}

	game = new Game;

	return true;
}

void Window::pollEvents() {
	SDL_Event event;

	if (SDL_WaitEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			m_Closed = true;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				m_Closed = true;
				break;
			case SDLK_r:
				game->printPositions();
				break;
			}
		case SDL_MOUSEBUTTONDOWN:
			game->LMB(event.button, game->getBoardState());
			break;
		case SDL_MOUSEBUTTONUP:
			game->LMB(event.button, game->getBoardState());
			break;
		default:
			break;
		}
	}
}