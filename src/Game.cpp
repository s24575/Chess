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

Game::~Game()
{

}

void Game::refreshTile(int x, int y)
{
	SDL_Color color = !((x + y) & 1) ? lightColor : darkColor;
	SDL_Rect tile = { x * 80, y * 80, 80, 80 };
	SDL_SetRenderDrawColor(Window::m_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(Window::m_Renderer, &tile);
}

void Game::refreshPiece(int x, int y)
{
	uint8_t currentPiece = currentBoardState.getBoard()[x][y];

	if (currentPiece)
	{
		SDL_Rect tile = { x * 80, y * 80, 80, 80 };
		SDL_RenderCopy(Window::m_Renderer, getPieceTexture(currentPiece), nullptr, &tile);
	}
}

void Game::refreshSquare(int x, int y)
{
	refreshTile(x, y);
	refreshPiece(x, y);	
}

void Game::refreshAllSquares()
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			refreshSquare(x, y);
		}
	}

	if (pickedUp)
	{
		highlightTile(movingPieceX, movingPieceY);
		for (int position : legalMoves)
		{
			int x = position % 8;
			int y = position / 8;
			highlightAttack(x, y);
		}
	}
}

void Game::highlightTile(int x, int y)
{
	SDL_Color currentColor = ((x + y) & 1) ? highlightDarkColor : highlightLightColor;
	SDL_Rect tile = { x * 80, y * 80, 80, 80 };
	SDL_SetRenderDrawColor(Window::m_Renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
	SDL_RenderFillRect(Window::m_Renderer, &tile);
	refreshPiece(x, y);
}

void Game::highlightAttack(int x, int y)
{	
	SDL_Point center = { x * 80 + 40, y * 80 + 40 };
	SDL_Color color = ((x + y) % 2) ? highlightDarkColor : highlightLightColor;
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

void Game::printPositions()
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			std::bitset<8> bits(currentBoardState.getBoard()[x][y]);
			std::cout << bits << ' ';
		}
		std::cout << '\n';
	}
	std::cout << '\n';
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
	if (currentBoardState.getBoard()[x][y] & currentBoardState.getCurrentTurn())
	{
		legalMoves = calculateLegalMoves(x, y, &currentBoardState);
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

	uint8_t cursorPiece = currentBoardState.getBoard()[movingPieceX][movingPieceY];

	if (currentBoardState.getBoard()[FinishX][FinishY] & currentBoardState.getCurrentTurn())
	{
		attemptPickupPiece(FinishX, FinishY);
	}
	else if (legalMoves.count(FinishX + (8 * FinishY)))
	{
		currentBoardState.movePiece(movingPieceX, movingPieceY, FinishX, FinishY);

		if (cursorPiece & Piece::pawn)
		{
			checkForSpecialPawnMoves(movingPieceY, FinishX, FinishY, &currentBoardState);
		}
		else if (cursorPiece & Piece::king)
		{
			checkForCastle(FinishX, FinishY, &currentBoardState);
			if (cursorPiece & Piece::white)
			{
				currentBoardState.setWhiteKing(FinishX + (8 * FinishY));
				currentBoardState.setWhiteShortCastle(false);
				currentBoardState.setWhiteLongCastle(false);
			}
			else
			{
				currentBoardState.setBlackKing(FinishX + (8 * FinishY));
				currentBoardState.setBlackShortCastle(false);
				currentBoardState.setBlackLongCastle(false);
			}
		}

		disableCastle(movingPieceX, movingPieceY, FinishX, FinishY, &currentBoardState);

		bool isCheckmate = checkForCheckmate(&currentBoardState);

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

std::unordered_set<int> Game::calculatePseudoLegalMoves(int x, int y, BoardState* boardstate)
{
	std::unordered_set<int> possibleMoves;

	int pieceType = boardstate->getBoard()[x][y];
	uint8_t** board = boardstate->getBoard();
	int piecePosition = x + (8 * y);
	uint8_t enemy = (pieceType & Piece::white) ? Piece::black : Piece::white;

	if (pieceType & Piece::pawn && pieceType & Piece::white)
	{ // replace with Piece::bottomPawn = black/white and Piece::upperPawn = black/white for different starting positions
		if (y == 6 && board[x][y - 1] == 0 && board[x][y - 2] == 0)
		{
			possibleMoves.insert(piecePosition - 16);
		}
		if (y > 0 && board[x][y - 1] == 0)
		{
			possibleMoves.insert(piecePosition - 8);
		}
		if (x > 0 && y > 0 && (board[x - 1][y - 1] & Piece::black || (x - 1) + 8 * (y - 1) == boardstate->getEnPassant()))
		{
			possibleMoves.insert(piecePosition - 9);
		}
		if (x < 7 && y > 0 && (board[x + 1][y - 1] & Piece::black || (x + 1) + 8 * (y - 1) == boardstate->getEnPassant()))
		{
			possibleMoves.insert(piecePosition - 7);
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::pawn && pieceType & Piece::black)
	{
		if (y == 1 && board[x][y + 1] == 0 && board[x][y + 2] == 0)
		{
			possibleMoves.insert(piecePosition + 16);
		}
		if (y < 7 && board[x][y + 1] == 0)
		{
			possibleMoves.insert(piecePosition + 8);

		}
		if (x > 0 && y < 7 && (board[x - 1][y + 1] & Piece::white || (x - 1) + 8 * (y + 1) == boardstate->getEnPassant()))
		{
			possibleMoves.insert(piecePosition + 7);
		}
		if (y < 7 && x < 7 && (board[x + 1][y + 1] & Piece::white || (x + 1) + 8 * (y + 1) == boardstate->getEnPassant()))
		{
			possibleMoves.insert(piecePosition + 9);
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::bishop)
	{
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				if (std::abs(x - i) == std::abs(y - j) && std::abs(y - j) > 0)
				{
					int n = std::abs(x - i);
					int directionX = (i - x) / n;
					int directionY = (j - y) / n;
					bool b = false;
					for (int k = 1; k < n + 1; ++k)
					{
						b = true;
						if (board[x + (k * directionX)][y + (k * directionY)] & enemy)
						{
							b = false;
							if (!(std::count(possibleMoves.begin(), possibleMoves.end(), x + (k * directionX) + (8 * (y + (k * directionY))))))
							{
								possibleMoves.insert(x + (k * directionX) + (8 * (y + (k * directionY))));
							}
							break;
						}
						if (board[x + (k * directionX)][y + (k * directionY)] != 0)
						{
							b = false;
							break;
						}
					}
					if (b)
					{
						possibleMoves.insert(i + (8 * j));
					}
				}
			}
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::knight)
	{
		if (x - 2 > -1 && y - 1 > -1)
		{
			if (board[x - 2][y - 1] == 0 || board[x - 2][y - 1] & enemy)
			{
				possibleMoves.insert(x - 2 + (8 * (y - 1)));
			}
		}
		if (x - 1 > -1 && y - 2 > -1)
		{
			if (board[x - 1][y - 2] == 0 || board[x - 1][y - 2] & enemy)
			{
				possibleMoves.insert(x - 1 + (8 * (y - 2)));
			}
		}
		if (!(x + 1 > 7) && y - 2 > -1)
		{
			if (board[x + 1][y - 2] == 0 || board[x + 1][y - 2] & enemy)
			{
				possibleMoves.insert(x + 1 + (8 * (y - 2)));
			}
		}
		if (!(x + 2 > 7) && y - 1 > -1)
		{
			if (board[x + 2][y - 1] == 0 || board[x + 2][y - 1] & enemy)
			{
				possibleMoves.insert(x + 2 + (8 * (y - 1)));
			}
		}
		if (!(x + 2 > 7) && !(y + 1 > 7))
		{
			if (board[x + 2][y + 1] == 0 || board[x + 2][y + 1] & enemy)
			{
				possibleMoves.insert(x + 2 + (8 * (y + 1)));
			}
		}
		if (!(x + 1 > 7) && !(y + 2 > 7))
		{
			if (board[x + 1][y + 2] == 0 || board[x + 1][y + 2] & enemy)
			{
				possibleMoves.insert(x + 1 + (8 * (y + 2)));
			}
		}
		if (x - 1 > -1 && !(y + 2 > 7))
		{
			if (board[x - 1][y + 2] == 0 || board[x - 1][y + 2] & enemy)
			{
				possibleMoves.insert(x - 1 + (8 * (y + 2)));
			}
		}
		if (x - 2 > -1 && !(y + 1 > 7))
		{
			if (board[x - 2][y + 1] == 0 || board[x - 2][y + 1] & enemy)
			{
				possibleMoves.insert(x - 2 + (8 * (y + 1)));
			}
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::rook)
	{
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				if ((i == x || j == y) && !(i == x && j == y))
				{
					int n = std::abs(x - i + y - j);
					int directionX = (i - x) / n;
					int directionY = (j - y) / n;
					bool b = false;
					for (int k = 1; k < n + 1; ++k)
					{
						b = true;
						if (board[x + (k * directionX)][y + (k * directionY)] & enemy)
						{
							b = false;
							if (!(std::count(possibleMoves.begin(), possibleMoves.end(), x + (k * directionX) + (8 * (y + (k * directionY))))))
							{
								possibleMoves.insert(x + (k * directionX) + (8 * (y + (k * directionY))));
							}
							break;
						}
						if (board[x + (k * directionX)][y + (k * directionY)] != 0)
						{
							b = false;
							break;
						}
					}
					if (b)
					{
						possibleMoves.insert(i + (8 * j));
					}
				}
			}
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::queen)
	{
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				if (std::abs(x - i) == std::abs(y - j) && std::abs(y - j) > 0)
				{
					int n = std::abs(x - i);
					int directionX = (i - x) / n;
					int directionY = (j - y) / n;
					bool b = false;
					for (int k = 1; k < n + 1; ++k)
					{
						b = true;
						if (board[x + (k * directionX)][y + (k * directionY)] & enemy)
						{
							b = false;
							if (!(std::count(possibleMoves.begin(), possibleMoves.end(), x + (k * directionX) + (8 * (y + (k * directionY))))))
							{
								possibleMoves.insert(x + (k * directionX) + (8 * (y + (k * directionY))));
							}
							break;
						}
						if (board[x + (k * directionX)][y + (k * directionY)] != 0)
						{
							b = false;
							break;
						}
					}
					if (b)
					{
						possibleMoves.insert(i + (8 * j));
					}
				}
			}
		}
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				if ((i == x || j == y) && !(i == x && j == y))
				{
					int n = std::abs(x - i + y - j);
					int directionX = (i - x) / n;
					int directionY = (j - y) / n;
					bool b = false;
					for (int k = 1; k < n + 1; ++k)
					{
						b = true;
						if (board[x + (k * directionX)][y + (k * directionY)] & enemy)
						{
							b = false;
							if (!(std::count(possibleMoves.begin(), possibleMoves.end(), x + (k * directionX) + (8 * (y + (k * directionY))))))
							{
								possibleMoves.insert(x + (k * directionX) + (8 * (y + (k * directionY))));
							}
							break;
						}
						if (board[x + (k * directionX)][y + (k * directionY)] != 0)
						{
							b = false;
							break;
						}
					}
					if (b)
					{
						possibleMoves.insert(i + (8 * j));
					}
				}
			}
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::king)
	{
		if (x - 1 > -1 && y - 1 > -1)
		{
			if (board[x - 1][y - 1] == 0 || board[x - 1][y - 1] & enemy)
			{
				possibleMoves.insert(x - 1 + (8 * (y - 1)));
			}
		}
		if (y - 1 > -1)
		{
			if (board[x][y - 1] == 0 || board[x][y - 1] & enemy)
			{
				possibleMoves.insert(x + (8 * (y - 1)));
			}
		}
		if (!(x + 1 > 7) && y - 1 > -1)
		{
			if (board[x + 1][y - 1] == 0 || board[x + 1][y - 1] & enemy)
			{
				possibleMoves.insert(x + 1 + (8 * (y - 1)));
			}
		}
		if (!(x + 1 > 7))
		{
			if (board[x + 1][y] == 0 || board[x + 1][y] & enemy)
			{
				possibleMoves.insert(x + 1 + (8 * y));
			}
		}
		if (!(x + 1 > 7) && !(y + 1 > 7))
		{
			if (board[x + 1][y + 1] == 0 || board[x + 1][y + 1] & enemy)
			{
				possibleMoves.insert(x + 1 + (8 * (y + 1)));
			}
		}
		if (!(y + 1 > 7))
		{
			if (board[x][y + 1] == 0 || board[x][y + 1] & enemy)
			{
				possibleMoves.insert(x + (8 * (y + 1)));
			}
		}
		if (x - 1 > -1 && !(y + 1 > 7))
		{
			if (board[x - 1][y + 1] == 0 || board[x - 1][y + 1] & enemy)
			{
				possibleMoves.insert(x - 1 + (8 * (y + 1)));
			}
		}
		if (x - 1 > -1)
		{
			if (board[x - 1][y] == 0 || board[x - 1][y] & enemy)
			{
				possibleMoves.insert(x - 1 + (8 * y));
			}
		}

		if (pieceType & Piece::white)
		{
			if (boardstate->getWhiteShortCastle())
			{
				if (board[5][7] == 0 && board[6][7] == 0)
				{
					possibleMoves.insert(x + 2 + (8 * y));
				}
			}
			if (boardstate->getWhiteLongCastle())
			{
				if (board[3][7] == 0 && board[2][7] == 0 && board[1][7] == 0)
				{
					possibleMoves.insert(x - 2 + (8 * y));
				}
			}
		}
		else if (pieceType & Piece::black)
		{
			if (boardstate->getBlackShortCastle())
			{
				if (board[5][0] == 0 && board[6][0] == 0)
				{
					possibleMoves.insert(x + 2 + (8 * y));
				}
			}
			if (boardstate->getBlackLongCastle())
			{
				if (board[3][0] == 0 && board[2][0] == 0 && board[1][0] == 0)
				{
					possibleMoves.insert(x - 2 + (8 * y));
				}
			}
		}

		return possibleMoves;
	}
	else
	{
		std::cout << pieceType << '\n';
		std::cout << "pseudoLegalMoves(int x, int y, uint8_t** board) Error: piece not found!\n";
	}
	return possibleMoves;
}

std::unordered_set<int> Game::calculateLegalMoves(int PieceX, int PieceY, BoardState* board)
{
	std::unordered_set<int> pseudoLegalMoves = calculatePseudoLegalMoves(PieceX, PieceY, board);
	std::unordered_set<int> validLegalMoves;

	uint8_t currentPiece = board->getBoard()[PieceX][PieceY];

	// use color mask and get the opposite
	uint8_t enemy = (currentPiece & (Piece::white | Piece::black)) ^ (Piece::white | Piece::black);

	if (currentPiece & Piece::king)
	{
		if (checkForCheck(board))
		{
			// disable castling
			if (currentPiece & Piece::white)
			{
				pseudoLegalMoves.erase(62);
				pseudoLegalMoves.erase(58);
			}
			else if (currentPiece & Piece::black)
			{
				pseudoLegalMoves.erase(6);
				pseudoLegalMoves.erase(2);
			}
		}
	}

	for (int move : pseudoLegalMoves)
	{
		bool isLegal = true;

		BoardState boardStateCopy(*board);

		int destinationX = move % 8;
		int destinationY = move / 8;

		boardStateCopy.movePiece(PieceX, PieceY, destinationX, destinationY);

		if (currentPiece & Piece::pawn)
		{
			checkForSpecialPawnMoves(PieceY, destinationX, destinationY, &boardStateCopy);
		}
		else if (currentPiece & Piece::king)
		{
			if (currentPiece & Piece::white)
			{
				boardStateCopy.setWhiteKing(destinationX + (8 * destinationY));
			}
			else if (currentPiece & Piece::black)
			{
				boardStateCopy.setBlackKing(destinationX + (8 * destinationY));
			}
			checkForCastle(destinationX, destinationY, &boardStateCopy);
		}

		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 8; ++x)
			{
				if (boardStateCopy.getBoard()[x][y] & enemy)
				{
					std::unordered_set<int> attackedSquares = calculatePseudoLegalMoves(x, y, &boardStateCopy);

					if (currentPiece & Piece::white)
					{
						if (attackedSquares.count(boardStateCopy.getWhiteKing()))
						{
							isLegal = false;
						}
					}
					if (currentPiece & Piece::black)
					{
						if (attackedSquares.count(boardStateCopy.getBlackKing()))
						{
							isLegal = false;
						}
					}
				}
			}
		}

		if (isLegal)
		{
			validLegalMoves.insert(move);
		}
	}

	return validLegalMoves;
}

void Game::checkForSpecialPawnMoves(int StartY, int FinishX, int FinishY, BoardState* boardState)
{
	uint8_t& currentPiece = boardState->getBoard()[FinishX][FinishY];

	if (currentPiece & Piece::white)
	{
		if (FinishX + (8 * FinishY) == boardState->getEnPassant())
		{
			boardState->getBoard()[FinishX][FinishY + 1] = 0;
		}
		boardState->setEnPassant(-1);
		if (FinishY == 4 && StartY == 6)
		{
			boardState->setEnPassant(FinishX + (8 * (FinishY + 1)));
		}
		else if (FinishY == 0)
		{
			if (boardState == &currentBoardState)
			{
				std::cout << "Choose your piece (queen/knight/rook/bishop): ";
				std::string promotionPiece;
				std::cin >> promotionPiece;

				if (promotionPiece == "queen")
				{
					currentPiece = Piece::white | Piece::queen;
				}
				else if (promotionPiece == "knight")
				{
					currentPiece = Piece::white | Piece::knight;
				}
				else if (promotionPiece == "rook")
				{
					currentPiece = Piece::white | Piece::rook;
				}
				else if (promotionPiece == "bishop")
				{
					currentPiece = Piece::white | Piece::bishop;
				}
			}
		}
	}
	else if (boardState->getBoard()[FinishX][FinishY] & Piece::black)
	{
		if (FinishX + (8 * FinishY) == boardState->getEnPassant())
		{
			boardState->getBoard()[FinishX][FinishY - 1] = 0;
		}
		boardState->setEnPassant(-1);
		if (FinishY == 3 && StartY == 1)
		{
			boardState->setEnPassant(FinishX + (8 * (FinishY - 1)));
		}
		else if (FinishY == 7)
		{
			if (boardState == &currentBoardState)
			{
				std::cout << "Choose your piece (queen/knight/rook/bishop): ";
				std::string promotionPiece;
				std::cin >> promotionPiece;
				if (promotionPiece == "queen")
				{
					boardState->getBoard()[FinishX][FinishY] = Piece::black | Piece::queen;
				}
				else if (promotionPiece == "knight")
				{
					boardState->getBoard()[FinishX][FinishY] = Piece::black | Piece::knight;
				}
				else if (promotionPiece == "rook")
				{
					boardState->getBoard()[FinishX][FinishY] = Piece::black | Piece::rook;
				}
				else if (promotionPiece == "bishop")
				{
					boardState->getBoard()[FinishX][FinishY] = Piece::black | Piece::bishop;
				}
			}
		}
	}
}

void Game::checkForCastle(int x, int y, BoardState* boardstate)
{
	int position = x + (8 * y);
	if (boardstate->getWhiteShortCastle() && position == 62)
	{
		boardstate->movePiece(7, 7, 5, 7);
	}
	else if (boardstate->getWhiteLongCastle() && position == 58)
	{
		boardstate->movePiece(0, 7, 3, 7);
	}
	else if (boardstate->getBlackShortCastle() && position == 6)
	{
		boardstate->movePiece(7, 0, 5, 0);
	}
	else if (boardstate->getBlackLongCastle() && position == 2)
	{
		boardstate->movePiece(0, 0, 3, 0);
	}
}

void Game::disableCastle(int StartX, int StartY, int FinishX, int FinishY, BoardState* boardstate)
{
	if ((FinishX == 7 && FinishY == 7) || (StartX == 7 && StartY == 7))
	{
		boardstate->setWhiteShortCastle(false);
	}
	if ((FinishX == 0 && FinishY == 7) || (StartX == 0 && StartY == 7))
	{
		boardstate->setWhiteLongCastle(false);
	}
	if ((FinishX == 7 && FinishY == 0) || (StartX == 7 && StartY == 0))
	{
		boardstate->setBlackShortCastle(false);
	}
	if ((FinishX == 0 && FinishY == 0) || (StartX == 0 && StartY == 0))
	{
		boardstate->setBlackLongCastle(false);
	}
}

bool Game::checkForCheck(BoardState* board)
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			uint8_t currentPiece = board->getBoard()[x][y];

			if (currentPiece & board->getOppositeTurn() && !(currentPiece & Piece::king))
			{
				std::unordered_set<int> attackedTiles = calculatePseudoLegalMoves(x, y, board);
				if (board->getCurrentTurn() & Piece::white && attackedTiles.count(board->getWhiteKing()) || board->getCurrentTurn() & Piece::black && attackedTiles.count(board->getBlackKing()))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool Game::checkForCheckmate(BoardState* board)
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			uint8_t currentPiece = board->getBoard()[x][y];

			if (currentPiece & board->getCurrentTurn())
			{
				auto moves = calculateLegalMoves(x, y, board);

				if (!moves.empty())
				{
					return false;
				}
			}
		}
	}

	return true;
}

// highlight checks
// test promotions in calculateLegalMoves
// test all possible moves in depth x
// 
// stockfish:
// ability to play as white or black
// repetition
// halfmoves
// fullmoves