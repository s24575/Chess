#include "BoardState.h"
#include "Game.h"
#include <iostream>

BoardState::~BoardState(){
	for (int x = 0; x < Game::Xboxes; x++) {
		delete[] board[x];
	}
	delete[] board;
}

uint8_t** BoardState::getBoard() {
	return board;
}

void BoardState::movePiece(int x, int y, uint8_t piece) {
	board[x][y] = piece;
}


void BoardState::setBoard(uint8_t** newBoard) {
	board = newBoard;
}