#pragma once
#include <stdint.h>

class BoardState {
public:
	~BoardState();

	uint8_t** getBoard();
	void movePiece(int StartX, int StartY, int FinishX, int FinishY);
	void setBoard(uint8_t**);
	
	void setWhiteKing(int position);
	void setBlackKing(int position);
	int getWhiteKing();
	int getBlackKing();
private:
	uint8_t** board;

	int whiteKingposition;
	int blackKingposition;
};