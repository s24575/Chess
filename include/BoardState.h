#pragma once

#include "Piece.h"

#include <cstdint>
#include <string>
#include <utility>
#include <unordered_set>

class BoardState
{
public:
	struct PairHash
	{
		std::size_t operator()(const std::pair<int8_t, int8_t>& p) const
		{
			return std::hash<int8_t>()(p.first) ^ (std::hash<int8_t>()(p.second) << 1);
		}
	};

	using Position = std::pair<int8_t, int8_t>;
	using PositionSet = std::unordered_set<Position, PairHash>;

	BoardState() = default;

	void movePiece(int StartX, int StartY, int FinishX, int FinishY);
	void loadFEN(const std::string& FEN);

	int calculateLegalMovesCount(int n);
	PositionSet calculatePseudoLegalMoves(int x, int y);
	PositionSet calculateLegalMoves(int x, int y);

	void checkForSpecialPawnMoves(int startY, int finishX, int finishY);
	void checkForCastle(int x, int y);
	void disableCastle(int x1, int y1, int x2, int y2);
	void removeIllegalCastling(PositionSet& possibleMoves, uint8_t piece);
	bool checkForCheck(uint8_t kingColor);
	bool checkForCheckmate();

public:
	inline uint8_t getAllyColor(uint8_t piece) const { return piece & Piece::colorMask; }
	inline uint8_t getEnemyColor(uint8_t piece) const { return (piece & Piece::colorMask) ^ Piece::colorMask; }

	inline uint8_t getPiece(int x, int y) const { return board[y][x]; }
	void setPiece(int x, int y, uint8_t piece) { board[y][x] = piece; }

	void setCurrentTurn(uint8_t turn) { currentTurn = turn; }
	inline uint8_t getCurrentTurn() const { return currentTurn; }
	void setOppositeTurn(uint8_t turn) { oppositeTurn = turn; }
	inline uint8_t getOppositeTurn() const { return oppositeTurn; }

	void setEnPassant(Position position) { enPassant = position; }
	inline Position getEnPassant() const { return enPassant; }

	void setWhiteKing(Position position) { whiteKing = position; }
	void setBlackKing(Position position) { blackKing = position; }
	inline Position getWhiteKing() const { return whiteKing; }
	inline Position getBlackKing() const { return blackKing; }

	void setWhiteShortCastle(bool updateCastle) { whiteShortCastle = updateCastle; }
	void setWhiteLongCastle(bool updateCastle) { whiteLongCastle = updateCastle; }
	void setBlackShortCastle(bool updateCastle) { blackShortCastle = updateCastle; }
	void setBlackLongCastle(bool updateCastle) { blackLongCastle = updateCastle; }
	inline bool getWhiteShortCastle() const { return whiteShortCastle; }
	inline bool getWhiteLongCastle() const { return whiteLongCastle; }
	inline bool getBlackShortCastle() const { return blackShortCastle; }
	inline bool getBlackLongCastle() const { return blackLongCastle; }

private:
	uint8_t board[8][8] = {0};

	uint8_t currentTurn = 0;
	uint8_t oppositeTurn = 0;

	Position enPassant = { -1, -1 };

	Position whiteKing = { -1, -1 };
	Position blackKing = { -1, -1 };

	bool whiteShortCastle = false;
	bool whiteLongCastle = false;
	bool blackShortCastle = false;
	bool blackLongCastle = false;
};