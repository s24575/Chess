#include "Game.h"
#include "Window.h"
#include "Piece.h"

#include <iostream>

#include <SDL2/SDL_image.h>

#include <bitset>
#include <algorithm>
#include <cmath>

#include <chrono>

static constexpr auto DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
static constexpr auto TEST_FEN1 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
static constexpr auto TEST_FEN2 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
static constexpr auto TEST_FEN3 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
static constexpr auto TEST_FEN3_MIRRORED = "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1";
static constexpr auto TEST_FEN4 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
static constexpr auto TEST_FEN5 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";

Game::Game()
{

}

void Game::init()
{
	currentBoardState.loadFEN(DEFAULT_FEN);
}

void Game::printMoveCount(int depth, BoardState& board)
{
	for (int i = 1; i <= depth; i++)
	{
		auto start_time = std::chrono::steady_clock::now();

		int count = board.calculateLegalMovesCount(i);

		auto end_time = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

		std::cout << "Depth: " << i << " Moves: " << count << " (" << duration.count() << " ms)" << '\n';
	}

	std::cout << '\n';
}

void Game::printAllTestsMoveCount(int depth)
{
	auto printTestMoveCount = [&](int depth, const std::string& fen)
	{
		BoardState board;
		board.loadFEN(fen);
		std::cout << fen << '\n';
		printMoveCount(depth, board);
	};

	printTestMoveCount(depth, DEFAULT_FEN);
	printTestMoveCount(depth, TEST_FEN1);
	printTestMoveCount(depth, TEST_FEN2);
	printTestMoveCount(depth, TEST_FEN3);
	printTestMoveCount(depth, TEST_FEN3_MIRRORED);
	printTestMoveCount(depth, TEST_FEN4);
	printTestMoveCount(depth, TEST_FEN5);
}

void Game::printCurrentMoveCount(int depth)
{
	printMoveCount(depth, currentBoardState);
}

void Game::printNextMoveCount()
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if (currentBoardState.getPiece(x, y) & currentBoardState.getCurrentTurn())
			{
				auto moves = currentBoardState.calculateLegalMoves(x, y);

				for (Position move : moves)
				{
					BoardState copy(currentBoardState);

					copy.movePiece(x, y, move.first, move.second);
					int count = copy.calculateLegalMovesCount(1);

					std::cout << (char)(x + 'a') << y + 1 << (char)(move.first + 'a') << (int)(move.second + 1) << ": " << count << '\n';
				}
			}
		}
	}

}

void Game::inputFEN()
{
	std::cout << "Please input a valid FEN position\n";
	std::string fen;
	std::getline(std::cin, fen);

	if (fen == "exit")
	{
		std::cout << "Operation cancelled.\n";
		return;
	}

	currentBoardState.loadFEN(fen);
	
	pickedUp = false;
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
		refreshTile(movingPiece.first, movingPiece.second, true);
		refreshPiece(movingPiece.first, movingPiece.second);

		for (Position position : legalMoves)
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
			movingPiece = { x, y };
		}
	}
}

void Game::attemptPlacePiece(int x, int y)
{
	Position target = { x, y };
	pickedUp = false;

	if (movingPiece == target)
		return;

	if (currentBoardState.getPiece(x, y) & currentBoardState.getCurrentTurn())
	{
		attemptPickupPiece(x, y);
	}
	else if (legalMoves.count(target))
	{
		currentBoardState.movePiece(movingPiece.first, movingPiece.second, x, y);

		if(currentBoardState.checkForCheckmate()){
			if (currentBoardState.getCurrentTurn() & Piece::black)
			{
				std::cout << "White won!\n";
			}
			else if (currentBoardState.getCurrentTurn() & Piece::white)
			{
				std::cout << "Black won!\n";
			}
		}
	}
}