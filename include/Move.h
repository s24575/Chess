#pragma once

#include "Piece.h"

#include <utility>
#include <cstdint>

struct Move
{
private:
	const uint16_t moveValue;

	static constexpr uint16_t startMask = 0b000000000111111;
	static constexpr uint16_t targetMask = 0b000111111000000;
	static constexpr uint16_t flagMask = 0b111000000000000;

public:
	struct Flag
	{
		static constexpr uint16_t None = 0;
		static constexpr uint16_t EnPassantCapture = 1;
		static constexpr uint16_t Castling = 2;
		static constexpr uint16_t PromoteToQueen = 3;
		static constexpr uint16_t PromoteToKnight = 4;
		static constexpr uint16_t PromoteToRook = 5;
		static constexpr uint16_t PromoteToBishop = 6;
		static constexpr uint16_t PawnTwoForward = 7;
	};
	
	Move(uint16_t moveValue);
	Move(uint16_t startSquare, uint16_t targetSquare);
	Move(uint16_t startSquare, uint16_t targetSquare, uint16_t flag);

	uint16_t getStartSquare();
	uint16_t getTargetSquare();
	uint16_t getFlag();

	bool isPromotion();
	uint8_t getPromotionPieceType();
};