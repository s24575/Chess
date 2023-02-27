#include "Piece.h"
#include "Window.h"

SDL_Texture* Piece::blackKingTexture;
SDL_Texture* Piece::whiteKingTexture;
SDL_Texture* Piece::blackKnightTexture;
SDL_Texture* Piece::whiteKnightTexture;
SDL_Texture* Piece::blackRookTexture;
SDL_Texture* Piece::whiteRookTexture;
SDL_Texture* Piece::blackBishopTexture;
SDL_Texture* Piece::whiteBishopTexture;
SDL_Texture* Piece::whiteQueenTexture;
SDL_Texture* Piece::blackQueenTexture;
SDL_Texture* Piece::whitePawnTexture;
SDL_Texture* Piece::blackPawnTexture;

uint8_t Piece::white;
uint8_t Piece::black;
uint8_t Piece::queen;
uint8_t Piece::king;
uint8_t Piece::bishop;
uint8_t Piece::knight;
uint8_t Piece::rook;
uint8_t Piece::pawn;

void Piece::init() {
	loadImages();
	loadTypes();
}

void Piece::loadImages() {
	SDL_Surface* surface;
	surface = IMG_Load("images/Lichess/bK.png");
	blackKingTexture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/Lichess/wK.png");
	whiteKingTexture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/Lichess/bQ.png");
	blackQueenTexture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/Lichess/wQ.png");
	whiteQueenTexture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/Lichess/bB.png");
	blackBishopTexture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/Lichess/wB.png");
	whiteBishopTexture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/Lichess/bN.png");
	blackKnightTexture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/Lichess/wN.png");
	whiteKnightTexture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/Lichess/bR.png");
	blackRookTexture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/Lichess/wR.png");
	whiteRookTexture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/Lichess/bP.png");
	blackPawnTexture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("images/Lichess/wP.png");
	whitePawnTexture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);
}

void Piece::destroyImages() {
	SDL_DestroyTexture(whitePawnTexture);
	SDL_DestroyTexture(blackPawnTexture);

	SDL_DestroyTexture(whiteRookTexture);
	SDL_DestroyTexture(blackRookTexture);

	SDL_DestroyTexture(whiteKnightTexture);
	SDL_DestroyTexture(blackKnightTexture);

	SDL_DestroyTexture(whiteBishopTexture);
	SDL_DestroyTexture(blackBishopTexture);

	SDL_DestroyTexture(whiteKingTexture);
	SDL_DestroyTexture(blackKingTexture);

	SDL_DestroyTexture(whiteQueenTexture);
	SDL_DestroyTexture(blackQueenTexture);
}

void Piece::loadTypes() {
	//first 6 bits for type
	pawn = 1;
	knight = 2;
	bishop = 4;
	rook = 8;
	queen = 16;
	king = 32;

	//last 2 bits for color
	white = 64;
	black = 128;
}