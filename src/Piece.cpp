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

uint8_t Piece::pieceMask;
uint8_t Piece::colorMask;

void Piece::init() {
	loadTypes();
	loadImages();
}

void Piece::loadTypes()
{
	//first 6 bits for type
	pawn =		0b00000001;
	knight =	0b00000010;
	bishop =	0b00000100;
	rook =		0b00001000;
	queen =		0b00010000;
	king =		0b00100000;

	//last 2 bits for color
	white =		0b01000000;
	black =		0b10000000;

	pieceMask = 0b00111111;
	colorMask = 0b11000000;
}

void Piece::loadImage(const char* filepath, SDL_Texture*& texture)
{
	SDL_Surface* surface = IMG_Load(filepath);
	texture = SDL_CreateTextureFromSurface(Window::m_Renderer, surface);
	SDL_FreeSurface(surface);
}

void Piece::loadImages() {
	loadImage("images/Lichess/wK.png", whiteKingTexture);
	loadImage("images/Lichess/bK.png", blackKingTexture);

	loadImage("images/Lichess/wQ.png", whiteQueenTexture);
	loadImage("images/Lichess/bQ.png", blackQueenTexture);

	loadImage("images/Lichess/wB.png", whiteBishopTexture);
	loadImage("images/Lichess/bB.png", blackBishopTexture);

	loadImage("images/Lichess/wN.png", whiteKnightTexture);
	loadImage("images/Lichess/bN.png", blackKnightTexture);

	loadImage("images/Lichess/wR.png", whiteRookTexture);
	loadImage("images/Lichess/bR.png", blackRookTexture);

	loadImage("images/Lichess/wP.png", whitePawnTexture);
	loadImage("images/Lichess/bP.png", blackPawnTexture);
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