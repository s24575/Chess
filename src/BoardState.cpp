#include "BoardState.h"
#include <iostream>

BoardState::BoardState()
{
	board = new uint8_t * [8];
	for (int i = 0; i < 8; ++i)
	{
		board[i] = new uint8_t[8]{ 0 };
	}
}
BoardState::BoardState(const BoardState& other) :
	enPassant(other.enPassant),
	whiteKing(other.whiteKing),
	blackKing(other.blackKing),
	whiteShortCastle(other.whiteShortCastle),
	whiteLongCastle(other.whiteLongCastle),
	blackShortCastle(other.blackShortCastle),
	blackLongCastle(other.blackLongCastle)
{
	board = new uint8_t * [8];
	for (int i = 0; i < 8; ++i)
	{
		board[i] = new uint8_t[8];
	}
	uint8_t** otherBoard = other.getBoard();
	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			board[y][x] = otherBoard[y][x];
		}
	}
}

BoardState::~BoardState()
{
	for (int x = 0; x < 8; x++)
	{
		delete[] board[x];
	}
	delete[] board;
}

void BoardState::movePiece(int StartX, int StartY, int FinishX, int FinishY)
{
	board[FinishX][FinishY] = board[StartX][StartY];
	board[StartX][StartY] = 0;
}