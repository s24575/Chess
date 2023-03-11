#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

class Piece
{
public:
	static void init();
	static void destroyImages();

private:
	static void loadTypes();
	static void loadImages();

	static void loadImage(const char* filepath, SDL_Texture*& texture);

public:
	static SDL_Texture* blackKingTexture;
	static SDL_Texture* whiteKingTexture;
	static SDL_Texture* blackKnightTexture;
	static SDL_Texture* whiteKnightTexture;
	static SDL_Texture* blackRookTexture;
	static SDL_Texture* whiteRookTexture;
	static SDL_Texture* blackBishopTexture;
	static SDL_Texture* whiteBishopTexture;
	static SDL_Texture* whiteQueenTexture;
	static SDL_Texture* blackQueenTexture;
	static SDL_Texture* whitePawnTexture;
	static SDL_Texture* blackPawnTexture;

	static uint8_t white;
	static uint8_t black;
	static uint8_t queen;
	static uint8_t king;
	static uint8_t bishop;
	static uint8_t knight;
	static uint8_t rook;
	static uint8_t pawn;

	static uint8_t pieceMask;
	static uint8_t colorMask;
};