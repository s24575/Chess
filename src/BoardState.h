#pragma once
#include <stdint.h>

class BoardState {
public:
	~BoardState();

	uint8_t** getBoard();
	void movePiece(int x, int y, uint8_t);
	void setBoard(uint8_t**);

private:
	uint8_t** board;
};