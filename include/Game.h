#pragma once

#include "BoardState.h"
#include <SDL.h>
#include <string>
#include <vector>

class Game
{
public:
	Game();
	~Game();

	SDL_Rect new_rect(int x, int y, int w, int h);
	void refreshTile(int x, int y);
	void refreshPiece(int x, int y);
	void refreshSquare(int x, int y);
	void refreshAllSquares();

	void hightlightPiece(int x, int y);
	void highlightAttack(int x, int y);

	void LoadFen(std::string FEN);
	SDL_Texture* checkPiece(uint8_t piece);
	void handleMouseButton(SDL_MouseButtonEvent& b);
	void attemptPickupPiece(int x, int y);
	void attemptPlacePiece(int x, int y);
	void printPositions();
	std::vector<int> calculatePseudoLegalMoves(int, int, BoardState* boardstate);
	void checkForSpecialPawnMoves(int StartX, int StartY, int FinishX, int FinishY, BoardState* boardstate);
	void checkForCastle(int x, int y, BoardState* board);
	void disableCastle(int StartX, int StartY, int FinishX, int FinishY, BoardState* boardstate);
	std::vector<int> LegalMoves(int x, int y);
	bool checkForCheck(int x, int y, BoardState* board);
	void checkForCheckmate();

	bool pickedUp = false;
private:
	BoardState* boardState = nullptr;

	SDL_Color lightColor = { 241, 217, 181, 255 };
	SDL_Color darkColor = { 181, 136, 99, 255 };

	SDL_Rect pieceTextures[8][8];

	std::vector<int> legalMoves;

	int movingPieceX = -1;
	int movingPieceY = -1;
	uint8_t turn;
	uint8_t oppositeColor;
};