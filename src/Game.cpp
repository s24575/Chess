#include "Game.h"
#include "Window.h"
#include "Piece.h"
#include <iostream>
#include <SDL_image.h>
#include <bitset>
#include <algorithm>
#include <cmath>

#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define TEST_FEN "r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R w KQkq - 3 2"
#define TEST_FEN2 "8/8/8/2k5/2pP4/8/B7/4K3 w - d3 0 3"

Game::Game() {
	Piece::init();

	boardState = new BoardState();
	LoadFen(DEFAULT_FEN);
	//LoadFen(TEST_FEN);
	//LoadFen(TEST_FEN2);
	refreshAllSquares();
}

Game::~Game() {}

SDL_Rect Game::new_rect(int x, int y, int w, int h) {
	SDL_Rect rect = { x, y, w, h };
	return rect;
}

void Game::refreshTile(int x, int y) {
	SDL_Color currentColor = (x + y) % 2 == 0 ? lightColor : darkColor;
	SDL_Rect tile = new_rect(x * 80, y * 80, 80, 80);
	SDL_SetRenderDrawColor(Window::m_Renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
	SDL_RenderFillRect(Window::m_Renderer, &tile);
}

void Game::refreshPiece(int x, int y) {
	if (boardState->getBoard()[x][y] != 0) {
		pieceTextures[x][y] = new_rect(x * 80, y * 80, 80, 80);
		SDL_RenderCopy(Window::m_Renderer, checkPiece(boardState->getBoard()[x][y]), nullptr, &pieceTextures[x][y]);
	}
}

void Game::refreshSquare(int x, int y) {
	refreshTile(x, y);
	refreshPiece(x, y);
}

void Game::refreshAllSquares() {
	for (int x = 0; x < 8; ++x) {
		for (int y = 0; y < 8; ++y) {
			refreshSquare(x, y);
		}
	}
	SDL_RenderPresent(Window::m_Renderer);
}

void Game::hightlightPiece(int x, int y) {
	SDL_Color currentColor;
	if ((x + y) % 2 == 0) {
		currentColor = { 130, 151, 105, 0 };
	}
	else {
		currentColor = { 100, 111, 64, 0 };
	}
	SDL_Rect tile = new_rect(x * 80, y * 80, 80, 80);
	SDL_SetRenderDrawColor(Window::m_Renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
	SDL_RenderFillRect(Window::m_Renderer, &tile);
	refreshPiece(x, y);
	SDL_RenderPresent(Window::m_Renderer);
}

void Game::highlightAttacks(int square) {
	SDL_Point center = { (square % 8) * 80 + 40, (square / 8) * 80 + 40 };
	SDL_Color color;
	int radius = 9;

	if (((square % 8) + (square / 8)) % 2 == 0) {
		color = { 130, 151, 105, 0 };
	}
	else {
		color = { 100, 111, 64, 0 };
	}

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

void Game::LoadFen(std::string FEN) {
	int index = 0;
	int x;

	uint8_t** board = boardState->getBoard();
	//for (int y = 0; y < 8; ++y) {// -> black at the bottom
	for (int y = 7; y > -1; --y) {
		x = 0;
		while (FEN[index] != '/' && FEN[index] != ' ') {
			if (isdigit(FEN[index])) {
				x += (FEN[index] - '0');
				index++;
			}
			else {
				switch (FEN[index]) {
				case 'p':
					board[x][y] = Piece::white | Piece::pawn;
					break;
				case 'P':
					board[x][y] = Piece::black | Piece::pawn;
					break;
				case 'r':
					board[x][y] = Piece::white | Piece::rook;
					break;
				case 'R':
					board[x][y] = Piece::black | Piece::rook;
					break;
				case 'b':
					board[x][y] = Piece::white | Piece::bishop;
					break;
				case 'B':
					board[x][y] = Piece::black | Piece::bishop;
					break;
				case 'n':
					board[x][y] = Piece::white | Piece::knight;
					break;
				case 'N':
					board[x][y] = Piece::black | Piece::knight;
					break;
				case 'k':
					board[x][y] = Piece::white | Piece::king;
					boardState->setWhiteKing(x + (8 * y));
					break;
				case 'K':
					board[x][y] = Piece::black | Piece::king;
					boardState->setBlackKing(x + (8 * y));
					break;
				case 'q':
					board[x][y] = Piece::white | Piece::queen;
					break;
				case 'Q':
					board[x][y] = Piece::black | Piece::queen;
					break;
				}
				index++;
				x++;
			}
		}
		index++;
	}

	if (FEN[index] == 'w') {
		turn = Piece::white;
		oppositeColor = Piece::black;
	}
	else if (FEN[index] == 'b') {
		turn = Piece::black;
		oppositeColor = Piece::white;
	}
	index += 2;

	boardState->setWhiteShortCastle(false);
	boardState->setWhiteLongCastle(false);
	boardState->setBlackShortCastle(false);
	boardState->setBlackLongCastle(false);

	while (FEN[index] != ' ') {
		switch (FEN[index]) {
		case 'K':
			boardState->setWhiteShortCastle(true);
			break;
		case 'Q':
			boardState->setWhiteLongCastle(true);
			break;
		case 'k':
			boardState->setBlackShortCastle(true);
			break;
		case 'q':
			boardState->setBlackLongCastle(true);
			break;
		}
		index++;
	}
	index++;

	if (FEN[index] != '-') {
		int x, y;
		x = int(FEN[index]) - 'a';
		index++;
		y = int(FEN[index]) - '1';
		boardState->setEnPassant(x + (8 * y));
	}
	index++;
}

SDL_Texture* Game::checkPiece(uint8_t piece) {
	if (piece & Piece::black) {
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
	else if (piece & Piece::white) {
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

void Game::printPositions() {
	for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {
			std::bitset<8> bits(boardState->getBoard()[x][y]);
			std::cout << bits << ' ';
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}

void Game::LMB(SDL_MouseButtonEvent& b) {
	int x, y, file, rank;
	if (b.button == SDL_BUTTON_LEFT) {
		SDL_GetMouseState(&x, &y);
		file = x / 80;
		rank = y / 80;

		if (!pickedUp) {
			attemptPickupPiece(file, rank);
		}
		else {
			attemptPlacePiece(file, rank);
		}
	}
}

void Game::attemptPickupPiece(int x, int y) {
	if (boardState->getBoard()[x][y] & turn)
	{
		legalMoves = LegalMoves(x, y);
		if (!legalMoves.empty()) {
			hightlightPiece(x, y);
			for (int i : legalMoves) {
				highlightAttacks(i);
			}
			SDL_RenderPresent(Window::m_Renderer);
			pickedUp = true;
			movingPieceX = x;
			movingPieceY = y;
		}
	}
}

void Game::attemptPlacePiece(int FinishX, int FinishY) {
	pickedUp = false;
	for (int i : legalMoves) {
		int AttackX = i % 8;
		int AttackY = i / 8;
		refreshSquare(AttackX, AttackY);
	}
	uint8_t cursorPiece = boardState->getBoard()[movingPieceX][movingPieceY];
	if (movingPieceX == FinishX && movingPieceY == FinishY) {
		pickedUp = true;
	}
	else if (boardState->getBoard()[FinishX][FinishY] & turn) {
		refreshSquare(movingPieceX, movingPieceY);
		attemptPickupPiece(FinishX, FinishY);
	}
	else if (std::count(legalMoves.begin(), legalMoves.end(), FinishX + (8 * FinishY))) {
		refreshTile(movingPieceX, movingPieceY);
		boardState->movePiece(movingPieceX, movingPieceY, FinishX, FinishY);
		refreshSquare(FinishX, FinishY);
		if (cursorPiece & Piece::pawn) {
			checkForSpecialPawnMoves(movingPieceX, movingPieceY, FinishX, FinishY, boardState);
		}
		else if (cursorPiece & Piece::king) {
			checkForCastle(FinishX, FinishY, boardState);
			if (cursorPiece & Piece::white) {
				boardState->setWhiteKing(FinishX + (8 * FinishY));
				boardState->setWhiteShortCastle(false);
				boardState->setWhiteLongCastle(false);
			}
			else {
				boardState->setBlackKing(FinishX + (8 * FinishY));
				boardState->setBlackShortCastle(false);
				boardState->setBlackLongCastle(false);
			}
		}
		SDL_RenderPresent(Window::m_Renderer);
		disableCastle(movingPieceX, movingPieceY, FinishX, FinishY, boardState);
		checkForCheckmate();

		if (turn == Piece::white) {
			turn = Piece::black;
			oppositeColor = Piece::white;
		}
		else if (turn == Piece::black) {
			turn = Piece::white;
			oppositeColor = Piece::black;
		}
	}
}

std::vector<int> Game::calculatePseudoLegalMoves(int x, int y, BoardState* boardstate) {
	std::vector<int> possibleMoves;
	int pieceType = boardstate->getBoard()[x][y];
	uint8_t** board = boardstate->getBoard();
	int piecePosition = x + (8 * y);
	uint8_t ally = 0;
	uint8_t enemy = 0;
	if (pieceType & Piece::white) {
		ally = Piece::white;
		enemy = Piece::black;
	}
	else if (pieceType & Piece::black) {
		ally = Piece::black;
		enemy = Piece::white;
	}

	if (pieceType & Piece::pawn && pieceType & Piece::white) { // replace with Piece::bottomPawn = black/white and Piece::upperPawn = black/white for different starting positions
		if (y == 6 && board[x][y - 1] == 0 && board[x][y - 2] == 0) {
			possibleMoves.push_back(piecePosition - 16);
		}
		if (y > 0 && board[x][y - 1] == 0) {
			possibleMoves.push_back(piecePosition - 8);
		}
		if (x > 0 && y > 0 && board[x - 1][y - 1] & Piece::black || (x - 1) + 8 * (y - 1) == boardstate->getEnPassant()) {
			possibleMoves.push_back(piecePosition - 9);
		}
		if (x < 7 && y > 0 && board[x + 1][y - 1] & Piece::black || (x + 1) + 8 * (y - 1) == boardstate->getEnPassant()) {
			possibleMoves.push_back(piecePosition - 7);
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::pawn && pieceType & Piece::black) {
		if (y == 1 && board[x][y + 1] == 0 && board[x][y + 2] == 0) {
			possibleMoves.push_back(piecePosition + 16);
		}
		if (y < 7 && board[x][y + 1] == 0) {
			possibleMoves.push_back(piecePosition + 8);

		}
		if (x > 0 && y < 7 && board[x - 1][y + 1] & Piece::white || (x - 1) + 8 * (y + 1) == boardstate->getEnPassant()) {
			possibleMoves.push_back(piecePosition + 7);
		}
		if (y < 7 && x < 7 && board[x + 1][y + 1] & Piece::white || (x + 1) + 8 * (y + 1) == boardstate->getEnPassant()) {
			possibleMoves.push_back(piecePosition + 9);
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::bishop) {
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (std::abs(x - i) == std::abs(y - j) && std::abs(y - j) > 0) {
					int n = std::abs(x - i);
					int directionX = (i - x) / n;
					int directionY = (j - y) / n;
					bool b = false;
					for (int k = 1; k < n + 1; ++k) {
						b = true;
						if (board[x + (k * directionX)][y + (k * directionY)] & enemy) {
							b = false;
							if (!(std::count(possibleMoves.begin(), possibleMoves.end(), x + (k * directionX) + (8 * (y + (k * directionY)))))) {
								possibleMoves.push_back(x + (k * directionX) + (8 * (y + (k * directionY))));
							}
							break;
						}
						if (board[x + (k * directionX)][y + (k * directionY)] != 0) {
							b = false;
							break;
						}
					}
					if (b) {
						possibleMoves.push_back(i + (8 * j));
					}
				}
			}
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::knight) {
		if (x - 2 > -1 && y - 1 > -1) {
			if (board[x - 2][y - 1] == 0 || board[x - 2][y - 1] & enemy) {
				possibleMoves.push_back(x - 2 + (8 * (y - 1)));
			}
		}
		if (x - 1 > -1 && y - 2 > -1) {
			if (board[x - 1][y - 2] == 0 || board[x - 1][y - 2] & enemy) {
				possibleMoves.push_back(x - 1 + (8 * (y - 2)));
			}
		}
		if (!(x + 1 > 7) && y - 2 > -1) {
			if (board[x + 1][y - 2] == 0 || board[x + 1][y - 2] & enemy) {
				possibleMoves.push_back(x + 1 + (8 * (y - 2)));
			}
		}
		if (!(x + 2 > 7) && y - 1 > -1) {
			if (board[x + 2][y - 1] == 0 || board[x + 2][y - 1] & enemy) {
				possibleMoves.push_back(x + 2 + (8 * (y - 1)));
			}
		}
		if (!(x + 2 > 7) && !(y + 1 > 7)) {
			if (board[x + 2][y + 1] == 0 || board[x + 2][y + 1] & enemy) {
				possibleMoves.push_back(x + 2 + (8 * (y + 1)));
			}
		}
		if (!(x + 1 > 7) && !(y + 2 > 7)) {
			if (board[x + 1][y + 2] == 0 || board[x + 1][y + 2] & enemy) {
				possibleMoves.push_back(x + 1 + (8 * (y + 2)));
			}
		}
		if (x - 1 > -1 && !(y + 2 > 7)) {
			if (board[x - 1][y + 2] == 0 || board[x - 1][y + 2] & enemy) {
				possibleMoves.push_back(x - 1 + (8 * (y + 2)));
			}
		}
		if (x - 2 > -1 && !(y + 1 > 7)) {
			if (board[x - 2][y + 1] == 0 || board[x - 2][y + 1] & enemy) {
				possibleMoves.push_back(x - 2 + (8 * (y + 1)));
			}
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::rook) {
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if ((i == x || j == y) && !(i == x && j == y)) {
					int n = std::abs(x - i + y - j);
					int directionX = (i - x) / n;
					int directionY = (j - y) / n;
					bool b = false;
					for (int k = 1; k < n + 1; ++k) {
						b = true;
						if (board[x + (k * directionX)][y + (k * directionY)] & enemy) {
							b = false;
							if (!(std::count(possibleMoves.begin(), possibleMoves.end(), x + (k * directionX) + (8 * (y + (k * directionY)))))) {
								possibleMoves.push_back(x + (k * directionX) + (8 * (y + (k * directionY))));
							}
							break;
						}
						if (board[x + (k * directionX)][y + (k * directionY)] != 0) {
							b = false;
							break;
						}
					}
					if (b) {
						possibleMoves.push_back(i + (8 * j));
					}
				}
			}
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::queen) {
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (std::abs(x - i) == std::abs(y - j) && std::abs(y - j) > 0) {
					int n = std::abs(x - i);
					int directionX = (i - x) / n;
					int directionY = (j - y) / n;
					bool b = false;
					for (int k = 1; k < n + 1; ++k) {
						b = true;
						if (board[x + (k * directionX)][y + (k * directionY)] & enemy) {
							b = false;
							if (!(std::count(possibleMoves.begin(), possibleMoves.end(), x + (k * directionX) + (8 * (y + (k * directionY)))))) {
								possibleMoves.push_back(x + (k * directionX) + (8 * (y + (k * directionY))));
							}
							break;
						}
						if (board[x + (k * directionX)][y + (k * directionY)] != 0) {
							b = false;
							break;
						}
					}
					if (b) {
						possibleMoves.push_back(i + (8 * j));
					}
				}
			}
		}
		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if ((i == x || j == y) && !(i == x && j == y)) {
					int n = std::abs(x - i + y - j);
					int directionX = (i - x) / n;
					int directionY = (j - y) / n;
					bool b = false;
					for (int k = 1; k < n + 1; ++k) {
						b = true;
						if (board[x + (k * directionX)][y + (k * directionY)] & enemy) {
							b = false;
							if (!(std::count(possibleMoves.begin(), possibleMoves.end(), x + (k * directionX) + (8 * (y + (k * directionY)))))) {
								possibleMoves.push_back(x + (k * directionX) + (8 * (y + (k * directionY))));
							}
							break;
						}
						if (board[x + (k * directionX)][y + (k * directionY)] != 0) {
							b = false;
							break;
						}
					}
					if (b) {
						possibleMoves.push_back(i + (8 * j));
					}
				}
			}
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::king) {
		if (x - 1 > -1 && y - 1 > -1) {
			if (board[x - 1][y - 1] == 0 || board[x - 1][y - 1] & enemy) {
				possibleMoves.push_back(x - 1 + (8 * (y - 1)));
			}
		}
		if (y - 1 > -1) {
			if (board[x][y - 1] == 0 || board[x][y - 1] & enemy) {
				possibleMoves.push_back(x + (8 * (y - 1)));
			}
		}
		if (!(x + 1 > 7) && y - 1 > -1) {
			if (board[x + 1][y - 1] == 0 || board[x + 1][y - 1] & enemy) {
				possibleMoves.push_back(x + 1 + (8 * (y - 1)));
			}
		}
		if (!(x + 1 > 7)) {
			if (board[x + 1][y] == 0 || board[x + 1][y] & enemy) {
				possibleMoves.push_back(x + 1 + (8 * y));
			}
		}
		if (!(x + 1 > 7) && !(y + 1 > 7)) {
			if (board[x + 1][y + 1] == 0 || board[x + 1][y + 1] & enemy) {
				possibleMoves.push_back(x + 1 + (8 * (y + 1)));
			}
		}
		if (!(y + 1 > 7)) {
			if (board[x][y + 1] == 0 || board[x][y + 1] & enemy) {
				possibleMoves.push_back(x + (8 * (y + 1)));
			}
		}
		if (x - 1 > -1 && !(y + 1 > 7)) {
			if (board[x - 1][y + 1] == 0 || board[x - 1][y + 1] & enemy) {
				possibleMoves.push_back(x - 1 + (8 * (y + 1)));
			}
		}
		if (x - 1 > -1) {
			if (board[x - 1][y] == 0 || board[x - 1][y] & enemy) {
				possibleMoves.push_back(x - 1 + (8 * y));
			}
		}

		if (pieceType & Piece::white) {
			if (boardState->getWhiteShortCastle()) {
				if (board[5][7] == 0 && board[6][7] == 0) {
					possibleMoves.push_back(x + 2 + (8 * y));
				}
			}
			if (boardState->getWhiteLongCastle()) {
				if (board[3][7] == 0 && board[2][7] == 0 && board[1][7] == 0) {
					possibleMoves.push_back(x - 2 + (8 * y));
				}
			}
		}
		else if (pieceType & Piece::black) {
			if (boardState->getBlackShortCastle()) {
				if (board[5][0] == 0 && board[6][0] == 0) {
					possibleMoves.push_back(x + 2 + (8 * y));
				}
			}
			if (boardState->getBlackLongCastle()) {
				if (board[3][0] == 0 && board[2][0] == 0 && board[1][0] == 0) {
					possibleMoves.push_back(x - 2 + (8 * y));
				}
			}
		}

		return possibleMoves;
	}
	else {
		std::cout << pieceType << '\n';
		std::cout << "pseudoLegalMoves(int x, int y, uint8_t** board) Error: piece not found!\n";
	}
	return possibleMoves;
}

std::vector<int> Game::LegalMoves(int PieceX, int PieceY) {
	std::vector<int> pseudoLegalMoves = calculatePseudoLegalMoves(PieceX, PieceY, boardState);
	std::vector<int> legalMoves;
	uint8_t enemy = -1;

	if (boardState->getBoard()[PieceX][PieceY] & Piece::white) {
		enemy = Piece::black;
	}
	else if (boardState->getBoard()[PieceX][PieceY] & Piece::black) {
		enemy = Piece::white;
	}
	
	if (boardState->getBoard()[PieceX][PieceY] & Piece::king) {
		if (checkForCheck(PieceX, PieceY, boardState)) {
			if (boardState->getBoard()[PieceX][PieceY] & Piece::white) {
				if (boardState->getWhiteShortCastle()) {
					pseudoLegalMoves.erase(std::find(pseudoLegalMoves.begin(), pseudoLegalMoves.end(), 62));
				}
				if (boardState->getWhiteLongCastle()) {
					pseudoLegalMoves.erase(std::find(pseudoLegalMoves.begin(), pseudoLegalMoves.end(), 58));
				}
			}
			else if (boardState->getBoard()[PieceX][PieceY] & Piece::black) {
				if (boardState->getBlackShortCastle()) {
					pseudoLegalMoves.erase(std::find(pseudoLegalMoves.begin(), pseudoLegalMoves.end(), 6));
				}
				if (boardState->getBlackLongCastle()) {
					pseudoLegalMoves.erase(std::find(pseudoLegalMoves.begin(), pseudoLegalMoves.end(), 2));
				}
			}
		}
	}

	bool isLegal;
	for (int move1 : pseudoLegalMoves) {
		isLegal = true;

		BoardState* boardStatecopy = new BoardState(*boardState);

		int destinationX = move1 % 8;
		int destinationY = move1 / 8;
		pseudoLegalMoves.pop_back();

		boardStatecopy->movePiece(PieceX, PieceY, destinationX, destinationY);

		if (boardStatecopy->getBoard()[destinationX][destinationY] & Piece::pawn) {
			checkForSpecialPawnMoves(PieceX, PieceY, destinationX, destinationY, boardStatecopy);
		}

		if (boardStatecopy->getBoard()[destinationX][destinationY] & Piece::king) {
			if (boardStatecopy->getBoard()[destinationX][destinationY] & Piece::white) {
				boardStatecopy->setWhiteKing(destinationX + (8 * destinationY));
				checkForCastle(destinationX, destinationY, boardStatecopy);
			}
			else if (boardStatecopy->getBoard()[destinationX][destinationY] & Piece::black) {
				boardStatecopy->setBlackKing(destinationX + (8 * destinationY));
				checkForCastle(destinationX, destinationY, boardStatecopy);
			}
		}

		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (boardStatecopy->getBoard()[j][i] & enemy) {
					std::vector<int> v = calculatePseudoLegalMoves(j, i, boardStatecopy);

					if (boardStatecopy->getBoard()[destinationX][destinationY] & Piece::white) {
						if (std::find(v.begin(), v.end(), boardStatecopy->getWhiteKing()) != v.end()) {
							isLegal = false;
						}
					}
					if (boardStatecopy->getBoard()[destinationX][destinationY] & Piece::black) {
						if (std::find(v.begin(), v.end(), boardStatecopy->getBlackKing()) != v.end()) {
							isLegal = false;
						}
					}
				}
			}
		}
		delete boardStatecopy;

		if (isLegal) {
			legalMoves.push_back(move1);
		}
	}

	return legalMoves;
}

void Game::checkForSpecialPawnMoves(int StartX, int StartY, int FinishX, int FinishY, BoardState* boardstate) {
	if (boardstate->getBoard()[FinishX][FinishY] & Piece::white) {
		if (FinishX + (8 * FinishY) == boardstate->getEnPassant()) {
			boardstate->getBoard()[FinishX][FinishY + 1] = 0b00000000;
			if (boardstate == boardState) {
				std::cout << FinishX << ' ' << FinishY + 1 << '\n';
				refreshTile(FinishX, FinishY + 1);
			}
		}
		boardstate->setEnPassant(-1);
		if (FinishY == 4 && StartY == 6) {
			boardstate->setEnPassant(FinishX + (8 * (FinishY + 1)));
		}
		else if (FinishY == 0) {
			if (boardstate == boardState) {
				std::cout << "Choose your piece (queen/knight/rook/bishop): ";
				std::string promotionPiece;
				std::cin >> promotionPiece;
				if (promotionPiece == "queen") {
					boardstate->getBoard()[FinishX][FinishY] = Piece::white | Piece::queen;
				}
				else if (promotionPiece == "knight") {
					boardstate->getBoard()[FinishX][FinishY] = Piece::white | Piece::knight;
				}
				else if (promotionPiece == "rook") {
					boardstate->getBoard()[FinishX][FinishY] = Piece::white | Piece::rook;
				}
				else if (promotionPiece == "bishop") {
					boardstate->getBoard()[FinishX][FinishY] = Piece::white | Piece::bishop;
				}
				refreshSquare(FinishX, FinishY);
			}
			else {

			}
		}
	}
	else if (boardstate->getBoard()[FinishX][FinishY] & Piece::black) {
		if (FinishX + (8 * FinishY) == boardstate->getEnPassant()) {
			boardstate->getBoard()[FinishX][FinishY - 1] = 0b00000000;
			if (boardstate == boardState) {
				std::cout << FinishX << ' ' << FinishY - 1 << '\n';
				refreshTile(FinishX, FinishY - 1);
			}
		}
		boardstate->setEnPassant(-1);
		if (FinishY == 3 && StartY == 1) {
			boardstate->setEnPassant(FinishX + (8 * (FinishY - 1)));
		}
		else if (FinishY == 7) {
			if (boardstate == boardState) {
				std::cout << "Choose your piece (queen/knight/rook/bishop): ";
				std::string promotionPiece;
				std::cin >> promotionPiece;
				if (promotionPiece == "queen") {
					boardstate->getBoard()[FinishX][FinishY] = Piece::black | Piece::queen;
				}
				else if (promotionPiece == "knight") {
					boardstate->getBoard()[FinishX][FinishY] = Piece::black | Piece::knight;
				}
				else if (promotionPiece == "rook") {
					boardstate->getBoard()[FinishX][FinishY] = Piece::black | Piece::rook;
				}
				else if (promotionPiece == "bishop") {
					boardstate->getBoard()[FinishX][FinishY] = Piece::black | Piece::bishop;
				}
				refreshSquare(FinishX, FinishY);
			}
			else {

			}
		}
	}
}

void Game::checkForCastle(int x, int y, BoardState* boardstate) {
	if (boardstate->getWhiteShortCastle() == true && x + (8 * y) == 62) {
		boardstate->movePiece(7, 7, 5, 7);
		if (boardstate == boardState) {
			refreshTile(7, 7);
			refreshPiece(5, 7);
		}
	}
	else if (boardstate->getWhiteLongCastle() == true && x + (8 * y) == 58) {
		boardstate->movePiece(0, 7, 3, 7);
		if (boardstate == boardState) {
			refreshTile(0, 7);
			refreshPiece(3, 7);
		}
	}
	else if (boardstate->getBlackShortCastle() == true && x + (8 * y) == 6) {
		boardstate->movePiece(7, 0, 5, 0);
		if (boardstate == boardState) {
			refreshTile(7, 0);
			refreshPiece(5, 0);
		}
	}
	else if (boardstate->getBlackLongCastle() == true && x + (8 * y) == 2) {
		boardstate->movePiece(0, 0, 3, 0);
		if (boardstate == boardState) {
			refreshTile(0, 0);
			refreshPiece(3, 0);
		}
	}
}

void Game::disableCastle(int StartX, int StartY, int FinishX, int FinishY, BoardState* boardstate) {
	if (boardstate->getWhiteShortCastle()) {
		if (FinishX == 7 && FinishY == 7 || StartX == 7 && StartY == 7) {
			boardstate->setWhiteLongCastle(false);
		}
	}
	if (boardstate->getWhiteLongCastle()) {
		if (FinishX == 0 && FinishY == 7 || StartX == 0 && StartY == 7) {
			boardstate->setWhiteLongCastle(false);
		}
	}
	if (boardstate->getBlackShortCastle()) {
		if (FinishX == 7 && FinishY == 0 || StartX == 7 && StartY == 0) {
			boardstate->setBlackShortCastle(false);
		}	
	}		
	if (boardstate->getBlackLongCastle()) {
		if (FinishX == 0 && FinishY == 0 || StartX == 0 && StartY == 0) {
			boardstate->setBlackShortCastle(false);
		}
	}
}

bool Game::checkForCheck(int PieceX, int PieceY, BoardState* board) {
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (board->getBoard()[j][i] & oppositeColor && !(board->getBoard()[j][i] & Piece::king)) {
				std::vector<int> v = LegalMoves(j, i);
				if (turn & Piece::white) {
					if (std::find(v.begin(), v.end(), board->getWhiteKing()) != v.end()) {
						return true;
					}
				}
				else if (turn & Piece::black) {
					if (std::find(v.begin(), v.end(), board->getBlackKing()) != v.end()) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

void Game::checkForCheckmate() {
	std::vector<int> enemyPieces;
	std::vector<int> attackingPieces;
	bool isCheck = false;
	int counter = 0;
	int positionX, positionY;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (boardState->getBoard()[j][i] & turn) {
				std::vector<int> v = calculatePseudoLegalMoves(j, i, boardState);
				if (turn & Piece::white) {
					if (std::find(v.begin(), v.end(), boardState->getBlackKing()) != v.end()) {
						attackingPieces.push_back(j + (8 * i));
						isCheck = true;
					}
				}
				else if (turn & Piece::black) {
					if (std::find(v.begin(), v.end(), boardState->getWhiteKing()) != v.end()) {
						attackingPieces.push_back(j + (8 * i));
						isCheck = true;
					}
				}
			}
			else if (boardState->getBoard()[j][i] & oppositeColor) {
				enemyPieces.push_back(j + (8 * i));
			}
		}
	}
	if (isCheck) {
		for (int position : enemyPieces) {
			positionX = position % 8;
			positionY = position / 8;
			std::vector<int> v = LegalMoves(positionX, positionY);
			if (!v.empty()) {
				counter += 1;
			}
		}
		if (!counter) {
			if (turn & Piece::white) {
				std::cout << "White won!\n";
			}
			else if (turn & Piece::black) {
				std::cout << "Black won!\n";
			}
		}
	}
}

// highlight checks
// test promotions in LegalMoves
// test all possible moves in depth x
// 
// stockfish:
// ability to play as white or black
// repetition
// halfmoves
// fullmoves