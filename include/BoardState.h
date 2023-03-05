#pragma once

#include <stdint.h>

class BoardState
{
public:
	BoardState();
	BoardState(const BoardState& other);
	~BoardState();

	void movePiece(int StartX, int StartY, int FinishX, int FinishY);

	inline uint8_t** getBoard() const { return board; }

	void setEnPassant(int position) { enPassant = position; }
	inline int getEnPassant() const { return enPassant; }

	void setWhiteKing(int position) { whiteKing = position; }
	void setBlackKing(int position) { blackKing = position; }
	inline int getWhiteKing() const { return whiteKing; }
	inline int getBlackKing() const { return blackKing; }

	void setWhiteShortCastle(bool updateCastle) { whiteShortCastle = updateCastle; }
	void setWhiteLongCastle(bool updateCastle) { whiteLongCastle = updateCastle; }
	void setBlackShortCastle(bool updateCastle) { blackShortCastle = updateCastle; }
	void setBlackLongCastle(bool updateCastle) { blackLongCastle = updateCastle; }
	inline bool getWhiteShortCastle() const { return whiteShortCastle; }
	inline bool getWhiteLongCastle() const { return whiteLongCastle; }
	inline bool getBlackShortCastle() const { return blackShortCastle; }
	inline bool getBlackLongCastle() const { return blackLongCastle; }

private:
	uint8_t** board = nullptr;

	int enPassant = -1;

	int whiteKing = -1;
	int blackKing = -1;

	bool whiteShortCastle = false;
	bool whiteLongCastle = false;
	bool blackShortCastle = false;
	bool blackLongCastle = false;
};