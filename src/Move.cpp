#include "Move.h"

Move::Move(uint16_t moveValue)
    : moveValue(moveValue)
{
}

Move::Move(uint16_t startSquare, uint16_t targetSquare)
    : moveValue(startMask | targetSquare << 6)
{
}

Move::Move(uint16_t startSquare, uint16_t targetSquare, uint16_t flag)
    : moveValue(startMask | targetSquare << 6 | flag << 12)
{
}

uint16_t Move::getStartSquare()
{
    return moveValue & startMask;
}

uint16_t Move::getTargetSquare()
{
    return (moveValue & targetMask) >> 6;
}

uint16_t Move::getFlag()
{
    return moveValue >> 12;
}

bool Move::isPromotion()
{
    uint16_t flag = getFlag();
    return flag == Flag::PromoteToQueen || flag == Flag::PromoteToKnight || flag == Flag::PromoteToRook || flag == Flag::PromoteToBishop;
}

uint8_t Move::getPromotionPieceType()
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