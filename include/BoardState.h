#pragma once

#include <stdint.h>
#include <string>
#include <unordered_set>

class BoardState
{
public:
	BoardState();
	BoardState(const BoardState& other);
	~BoardState();

	void movePiece(int StartX, int StartY, int FinishX, int FinishY);
	void loadFEN(const std::string& FEN);

	std::unordered_set<int> calculatePseudoLegalMoves(int x, int y);
	void checkForSpecialPawnMoves(int startY, int finishX, int finishY);
	void checkForCastle(int x, int y);
	void disableCastle(int x1, int y1, int x2, int y2);
	std::unordered_set<int> calculateLegalMoves(int x, int y);
	bool checkForCheck();
	bool checkForCheckmate();

public:
	inline uint8_t getPiece(int x, int y) const { return board[y][x]; }
	void setPiece(int x, int y, uint8_t piece) { board[y][x] = piece; }

	inline uint8_t** getBoard() const { return board; }

	void setCurrentTurn(uint8_t turn) { currentTurn = turn; }
	inline uint8_t getCurrentTurn() const { return currentTurn; }
	void setOppositeTurn(uint8_t turn) { oppositeTurn = turn; }
	inline uint8_t getOppositeTurn() const { return oppositeTurn; }

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

	uint8_t currentTurn = 0;
	uint8_t oppositeTurn = 0;

	int enPassant = -1;

	int whiteKing = -1;
	int blackKing = -1;

	bool whiteShortCastle = false;
	bool whiteLongCastle = false;
	bool blackShortCastle = false;
	bool blackLongCastle = false;
};