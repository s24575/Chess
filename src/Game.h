#pragma once

#include "BoardState.h"
#include <SDL2/SDL.h>
#include <string>
#include <vector>

class Game
{
public:
	Game();
	~Game();

	void init();

	void refreshTile(int x, int y);
	void refreshPiece(int x, int y);
	void refreshSquare(int x, int y);

	void LoadFen(std::string, BoardState*);
	SDL_Rect new_rect(int, int, int, int);
	SDL_Texture* checkPiece(uint8_t);
	void LMB(SDL_MouseButtonEvent&, BoardState*);
	void pickupPiece(int, int, BoardState*);
	void placePiece(int, int, BoardState*);
	BoardState* getBoardState();
	void printPositions();
	std::vector<int> pseudoLegalMoves(int, int, uint8_t** board);
	void checkForCastle(int x, int y, BoardState* board);
	void disableCastle(int x, int y);
	std::vector<int> LegalMoves(int x, int y, std::vector<int>);
	bool checkForCheck(int x, int y, BoardState* board);
	void checkForCheckmate();

	static int Xboxes;
	static int Yboxes;
	static int boxXwidth;
	static int boxYheight;

	uint8_t cursorPiece;
	bool pickedUp = false;
private:
	BoardState* boardState;

	SDL_Color lightColor = { 241, 217, 181, 255 };
	SDL_Color darkColor = { 181, 136, 99, 255 };

	SDL_Rect pieceTextures[8][8];

	int movingPieceX;
	int movingPieceY;
	int enPassant;
	uint8_t turn;
	uint8_t oppositeColor;

	int whiteKingPosition = 60;
	int blackKingPosition = 4;
	bool whiteShortCastle;
	bool whiteLongCastle;
	bool blackShortCastle;
	bool blackLongCastle;
};