#include "Game.h"
#include "Window.h"
#include "Piece.h"

#include <iostream>

#include <SDL2/SDL_image.h>

#include <bitset>
#include <algorithm>
#include <cmath>

constexpr auto DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr auto TEST_FEN = "r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R w KQkq - 3 2";
constexpr auto TEST_FEN2 = "8/8/8/2k5/2pP4/8/B7/4K3 w - d3 0 3";

Game::Game()
{

}

void Game::init()
{
	currentBoardState.loadFEN(DEFAULT_FEN);
}

void Game::printMoveCount()
{
	int depth = 2;

	std::cout << currentBoardState.calculateLegalMovesCount(depth) << '\n';
}

void Game::refreshTile(int x, int y, bool highlight = false)
{
	SDL_Color color = ((x + y) & 1) ? darkColor : lightColor;

	if (highlight)
	{
		color = ((x + y) & 1) ? highlightDarkColor : highlightLightColor;
	}

	SDL_Rect tile = { x * 80, y * 80, 80, 80 };
	SDL_SetRenderDrawColor(Window::m_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(Window::m_Renderer, &tile);
}

void Game::refreshPiece(int x, int y)
{
	uint8_t currentPiece = currentBoardState.getPiece(x, y);

	if (currentPiece)
	{
		SDL_Rect tile = { x * 80, y * 80, 80, 80 };
		SDL_RenderCopy(Window::m_Renderer, getPieceTexture(currentPiece), nullptr, &tile);
	}
}

void Game::refreshAllSquares()
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			refreshTile(x, y);
			refreshPiece(x, y);
		}
	}

	if (pickedUp)
	{
		refreshTile(movingPieceX, movingPieceY, true);
		refreshPiece(movingPieceX, movingPieceY);

		for (BoardState::Position position : legalMoves)
		{
			highlightAttack(position.first, position.second);
		}
	}
}

void Game::highlightAttack(int x, int y)
{
	SDL_Point center = { x * 80 + 40, y * 80 + 40 };
	SDL_Color color = ((x + y) & 1) ? highlightDarkColor : highlightLightColor;
	int radius = 9;

	SDL_SetRenderDrawColor(Window::m_Renderer, color.r, color.g, color.b, color.a);
	for (int w = 0; w < radius * 2; w++)
	{
		for (int h = 0; h < radius * 2; h++)
		{
			int dx = radius - w;
			int dy = radius - h;
			if ((dx * dx + dy * dy) <= (radius * radius))
			{
				SDL_RenderDrawPoint(Window::m_Renderer, center.x + dx, center.y + dy);
			}
		}
	}
}

SDL_Texture* Game::getPieceTexture(uint8_t piece)
{
	if (piece & Piece::black)
	{
		if (piece & Piece::pawn)
			return Piece::blackPawnTexture;
		else if (piece & Piece::knight)
			return Piece::blackKnightTexture;
		else if (piece & Piece::bishop)
			return Piece::blackBishopTexture;
		else if (piece & Piece::rook)
			return Piece::blackRookTexture;
		else if (piece & Piece::queen)
			return Piece::blackQueenTexture;
		else if (piece & Piece::king)
			return Piece::blackKingTexture;
	}
	else if (piece & Piece::white)
	{
		if (piece & Piece::pawn)
			return Piece::whitePawnTexture;
		else if (piece & Piece::knight)
			return Piece::whiteKnightTexture;
		else if (piece & Piece::bishop)
			return Piece::whiteBishopTexture;
		else if (piece & Piece::rook)
			return Piece::whiteRookTexture;
		else if (piece & Piece::queen)
			return Piece::whiteQueenTexture;
		else if (piece & Piece::king)
			return Piece::whiteKingTexture;
	}
	return nullptr;
}

void Game::handleMouseButton(SDL_MouseButtonEvent& b)
{
	if (b.button == SDL_BUTTON_LEFT)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		x /= 80;
		y /= 80;

		if (pickedUp)
		{
			attemptPlacePiece(x, y);
		}
		else
		{
			attemptPickupPiece(x, y);
		}
	}
	else if (b.button == SDL_BUTTON_RIGHT)
	{
		pickedUp = false;
	}
}

void Game::attemptPickupPiece(int x, int y)
{
	if (currentBoardState.getPiece(x, y) & currentBoardState.getCurrentTurn())
	{
		legalMoves = currentBoardState.calculateLegalMoves(x, y);
		if (!legalMoves.empty())
		{
			pickedUp = true;
			movingPieceX = x;
			movingPieceY = y;
		}
	}
}

void Game::attemptPlacePiece(int FinishX, int FinishY)
{
	pickedUp = false;

	if (movingPieceX == FinishX && movingPieceY == FinishY)
		return;

	if (currentBoardState.getPiece(FinishX, FinishY) & currentBoardState.getCurrentTurn())
	{
		attemptPickupPiece(FinishX, FinishY);
	}
	else if (legalMoves.count({ FinishX, FinishY }))
	{
		currentBoardState.movePiece(movingPieceX, movingPieceY, FinishX, FinishY);

		bool isCheckmate = currentBoardState.checkForCheckmate();

		if(isCheckmate){
			if (currentBoardState.getOppositeTurn() & Piece::white)
			{
				std::cout << "White won!\n";
			}
			else if (currentBoardState.getOppositeTurn() & Piece::black)
			{
				std::cout << "Black won!\n";
			}
		}
	}
}

// highlight checks
// test all possible moves in depth x
// 
// stockfish:
// ability to play as white or black
// repetition
// halfmoves
// fullmoves