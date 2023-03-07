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
	~Game();

	void refreshTile(int x, int y);
	void refreshPiece(int x, int y);
	void refreshSquare(int x, int y);
	void refreshAllSquares();

	void highlightTile(int x, int y);
	void highlightAttack(int x, int y);

	void loadFEN(std::string FEN);
	SDL_Texture* checkPiece(uint8_t piece);
	void handleMouseButton(SDL_MouseButtonEvent& b);
	void attemptPickupPiece(int x, int y);
	void attemptPlacePiece(int x, int y);
	void printPositions();

	std::unordered_set<int> calculatePseudoLegalMoves(int x, int y, BoardState* board);
	void checkForSpecialPawnMoves(int startX, int startY, int finishX, int finishY, BoardState* board);
	void checkForCastle(int x, int y, BoardState* board);
	void disableCastle(int StartX, int StartY, int FinishX, int FinishY, BoardState* board);
	std::unordered_set<int> calculateLegalMoves(int x, int y, BoardState* board);
	bool checkForCheck(BoardState* board);
	bool checkForCheckmate(BoardState* board);

	bool pickedUp = false;
private:
	BoardState* currentBoardState = nullptr;

	SDL_Color lightColor = { 241, 217, 181, 255 };
	SDL_Color darkColor = { 181, 136, 99, 255 };

	SDL_Color highlightLightColor = { 130, 151, 105, 0 };
	SDL_Color highlightDarkColor = { 100, 111, 64, 0 };



	SDL_Rect pieceTextures[8][8];

	std::unordered_set<int> legalMoves;

	int movingPieceX = -1;
	int movingPieceY = -1;
	uint8_t turn;
	uint8_t oppositeColor;
};