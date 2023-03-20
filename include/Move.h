#pragma once

#include "Piece.h"

#include <utility>
#include <cstdint>

struct Move
{
private:
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

	const uint16_t moveValue;

	static constexpr uint16_t startMask = 0b000000000111111;
	static constexpr uint16_t targetMask = 0b000111111000000;
	static constexpr uint16_t flagMask = 0b111000000000000;

public:
	Move(uint16_t moveValue)
		: moveValue(moveValue)
	{}

	Move(uint16_t startSquare, uint16_t targetSquare)
		: moveValue(startMask | targetSquare << 6)
	{}

	Move(uint16_t startSquare, uint16_t targetSquare, uint16_t flag)
		: moveValue(startMask | targetSquare << 6 | flag << 12)
	{}

	uint16_t getStartSquare()
	{
		return moveValue & startMask;
	}

	uint16_t getTargetSquare()
	{
		return (moveValue & targetMask) >> 6;
	}

	uint16_t getFlag()
	{
		return moveValue >> 12;
	}

	bool isPromotion()
	{
		uint16_t flag = getFlag();
		return flag == Flag::PromoteToQueen || flag == Flag::PromoteToKnight || flag == Flag::PromoteToRook || flag == Flag::PromoteToBishop;
	}

	uint8_t getPromotionPieceType()
	{
		switch (getFlag())
		{
			case Flag::PromoteToQueen:
				return Piece::queen;
			case Flag::PromoteToKnight:
				return Piece::knight;
			case Flag::PromoteToRook:
				return Piece::rook;
			case Flag::PromoteToBishop:
				return Piece::bishop;
			default:
				return Piece::none;
		}
	}
};