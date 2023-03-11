#pragma once

#include "BoardState.h"

#include <SDL2/SDL.h>

#include <string>
#include <vector>
#include <unordered_set>

class Game
{
public:
	Game();

	void init();

	void refreshTile(int x, int y, bool highlight);
	void refreshPiece(int x, int y);
	void refreshSquare(int x, int y);
	void refreshAllSquares();
	void highlightAttack(int x, int y);

	SDL_Texture* getPieceTexture(uint8_t piece);
	void handleMouseButton(SDL_MouseButtonEvent& b);
	void attemptPickupPiece(int x, int y);
	void attemptPlacePiece(int x, int y);

	bool pickedUp = false;
private:
	BoardState currentBoardState;

	SDL_Color lightColor = { 241, 217, 181, 255 };
	SDL_Color darkColor = { 181, 136, 99, 255 };

	SDL_Color highlightLightColor = { 130, 151, 105, 0 };
	SDL_Color highlightDarkColor = { 100, 111, 64, 0 };

	std::unordered_set<int> legalMoves;

	int movingPieceX = -1;
	int movingPieceY = -1;
};