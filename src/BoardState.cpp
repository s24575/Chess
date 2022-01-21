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

void BoardState::movePiece(int StartX, int StartY, int FinishX, int FinishY) {
	board[FinishX][FinishY] = board[StartX][StartY];
	board[StartX][StartY] = 0b00000000;
}

void BoardState::setBoard(uint8_t** newBoard) {
	board = newBoard;
}

void BoardState::setWhiteKing(int position){
	whiteKingposition = position;
}

void BoardState::setBlackKing(int position){
	blackKingposition = position;
}

int BoardState::getWhiteKing(){
	return whiteKingposition;
}

int BoardState::getBlackKing(){
	return blackKingposition;
}