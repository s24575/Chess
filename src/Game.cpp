#include "Game.h"
#include "Window.h"
#include "Piece.h"
#include <iostream>
#include <SDL2/SDL_image.h>
#include <bitset>
#include <algorithm>
#include <cmath>

#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define TEST_FEN "r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R w KQkq - 3 2"
#define TEST_FEN2 "8/8/8/2k5/2pP4/8/B7/4K3 w - d3 0 3"

int Game::Xboxes;
int Game::Yboxes;
int Game::boxXwidth;
int Game::boxYheight;

Game::Game() {
	init();
}

Game::~Game() {}

void Game::init() {
	Piece::init();
	Xboxes = 8;
	Yboxes = 8;
	boxXwidth = Window::m_Width/Xboxes;
	boxYheight = Window::m_Height/Yboxes;

	boardState = new BoardState();
	
	uint8_t** board = new uint8_t * [Xboxes];
	for (int i = 0; i < Xboxes; ++i) {
		board[i] = new uint8_t[Yboxes];
	}
	for (int x = 0; x < Xboxes; ++x) {
		for (int y = 0; y < Yboxes; ++y) {	

			board[x][y] = 0;
		}
	}
	boardState->setBoard(board);
	LoadFen(DEFAULT_FEN, boardState);
	//LoadFen(TEST_FEN, boardState);
	//LoadFen(TEST_FEN2, boardState);

	for (int i = 0; i < 8; ++i){
		for (int j = 0; j < 8; ++j){
			if (boardState->getBoard()[j][i] & (Piece::white | Piece::king)){
				boardState->setWhiteKing(i + (8 * j));
			}
			else if (boardState->getBoard()[j][i] & (Piece::black | Piece::king)){
				boardState->setBlackKing(i + (8 * j));
			}
		}
	}

	for (int x = 0; x < 8; ++x) {
		for (int y = 0; y < 8; ++y) {
			refreshSquare(x, y);
		}
	}
	SDL_RenderPresent(Window::m_Renderer);
}

BoardState* Game::getBoardState() {
	return boardState;
}

SDL_Rect Game::new_rect(int x, int y, int w, int h) {
	SDL_Rect rect = { x, y, w, h };
	return rect;
}

void Game::refreshTile(int x, int y) {
	SDL_Color currentColor = (x + y) % 2 == 0 ? lightColor : darkColor;
	SDL_Rect tile = new_rect(x * boxXwidth, y * boxYheight, boxXwidth, boxYheight);
	SDL_SetRenderDrawColor(Window::m_Renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
	SDL_RenderFillRect(Window::m_Renderer, &tile);
}

void Game::refreshPiece(int x, int y) {
	if (boardState->getBoard()[x][y] != 0) {
		pieceTextures[x][y] = new_rect(x * boxXwidth, y * boxYheight, 80, 80);
		SDL_RenderCopy(Window::m_Renderer, checkPiece(boardState->getBoard()[x][y]), nullptr, &pieceTextures[x][y]);
	}
}

void Game::refreshSquare(int x, int y) {
	refreshTile(x, y);
	refreshPiece(x, y);
}

void Game::LoadFen(std::string FEN, BoardState* currentBoardState) {
	int index = 0;
	int x;

	uint8_t** board = currentBoardState->getBoard();
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
					break;
				case 'K':
					board[x][y] = Piece::black | Piece::king;
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

	whiteShortCastle = false;
	whiteLongCastle = false;
	blackShortCastle = false;
	blackLongCastle = false;

	while (FEN[index] != ' ') {
		switch (FEN[index]) {
		case 'K':
			whiteShortCastle = true;
			break;
		case 'Q':
			whiteLongCastle = true;
			break;
		case 'k':
			blackShortCastle = true;
			break;
		case 'q':
			blackLongCastle = true;
			break;
		}
		index++;
	}
	index++;

	if (FEN[index] != '-') {
		int x, y;
		x = int(FEN[index]) - 97;
		index++;
		y = int(FEN[index]) - 49;
		enPassant = x + (8 * y);
	}
	else {
		enPassant = -1;
	}
	index++;
}

SDL_Texture* Game::checkPiece(uint8_t board) {
	if (board & Piece::black) {
		if (board & Piece::pawn)
			return Piece::blackPawnTexture;
		else if (board & Piece::knight)
			return Piece::blackKnightTexture;
		else if (board & Piece::bishop)
			return Piece::blackBishopTexture;
		else if (board & Piece::rook)
			return Piece::blackRookTexture;
		else if (board & Piece::queen)
			return Piece::blackQueenTexture;
		else if (board & Piece::king)
			return Piece::blackKingTexture;
		else {
			std::cout << "Black piece not found!\n";
		}
	}
	else if (board & Piece::white) {
		if (board & Piece::pawn)
			return Piece::whitePawnTexture;
		else if (board & Piece::knight)
			return Piece::whiteKnightTexture;
		else if (board & Piece::bishop)
			return Piece::whiteBishopTexture;
		else if (board & Piece::rook)
			return Piece::whiteRookTexture;
		else if (board & Piece::queen)
			return Piece::whiteQueenTexture;
		else if (board & Piece::king)
			return Piece::whiteKingTexture;
		else {
			std::cout << "White piece not found!\n";
		}
	}
	else {
		std::cout << "checkPiece(uint8_t board) Error: piece not found!\n";
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

void Game::LMB(SDL_MouseButtonEvent& b, BoardState* currentBoardState) {
	int x, y, file, rank;
	if (b.button == SDL_BUTTON_LEFT) {
		SDL_GetMouseState(&x, &y);
		file = x / boxXwidth;
		rank = y / boxYheight;
		
		if (!pickedUp) {
			pickupPiece(file, rank, currentBoardState);
		}
		else {
			placePiece(file, rank, currentBoardState);
		}
	}
}

void Game::pickupPiece(int x, int y, BoardState* currentBoardState) {
	if (currentBoardState->getBoard()[x][y] != 0 && currentBoardState->getBoard()[x][y] & turn && checkForCheck(x, y, currentBoardState))
	{
		pickedUp = true;
		movingPieceX = x;
		movingPieceY = y;
		cursorPiece = currentBoardState->getBoard()[x][y];

		refreshTile(x, y);
		SDL_RenderPresent(Window::m_Renderer);
	}
}

void Game::placePiece(int x, int y, BoardState* currentBoardState) {
	pickedUp = false;
	std::vector<int> v = LegalMoves(movingPieceX, movingPieceY, pseudoLegalMoves(movingPieceX, movingPieceY, currentBoardState->getBoard()));

	if ((x != movingPieceX || y != movingPieceY) && std::count(v.begin(), v.end(), x + (8 * y)) && currentBoardState->getBoard()[movingPieceX][movingPieceY] & turn) {

		//currentBoardState->movePiece(movingPieceX, movingPieceY, 0b00000000);

		if (cursorPiece & Piece::pawn) {
			if (cursorPiece & Piece::white) {
				if (x + (8 * y) == enPassant) {
					refreshTile(x, y + 1);
					currentBoardState->getBoard()[x][y + 1] = 0b00000000;
				}
				enPassant = -1;
				if (y == 4 && movingPieceY == 6) {
					enPassant = x + (8 * (y + 1));
				}
				else if (y == 0) {
					std::cout << "Choose your piece (queen/knight/rook/bishop): ";
					std::string promotionPiece;
					std::cin >> promotionPiece;
					if (promotionPiece == "queen") {
						cursorPiece = Piece::white | Piece::queen;
					}
					else if (promotionPiece == "knight") {
						cursorPiece = Piece::white | Piece::knight;
					}
					else if (promotionPiece == "rook") {
						cursorPiece = Piece::white | Piece::rook;
					}
					else if (promotionPiece == "bishop") {
						cursorPiece = Piece::white | Piece::bishop;
					}
				}
			}
			else if (cursorPiece & Piece::black) {
				if (x + (8 * y) == enPassant) {
					refreshTile(x, y - 1);
					currentBoardState->getBoard()[x][y - 1] = 0b00000000;
				}
				enPassant = -1;
				if (y == 3 && movingPieceY == 1) {
					enPassant = x + (8 * (y - 1));
				}
				else if (y == 7) {
					std::cout << "Choose your piece (queen/knight/rook/bishop): ";
					std::string promotionPiece;
					std::cin >> promotionPiece;
					if (promotionPiece == "queen") {
						cursorPiece = Piece::black | Piece::queen;
					}
					else if (promotionPiece == "knight") {
						cursorPiece = Piece::black | Piece::knight;
					}
					else if (promotionPiece == "rook") {
						cursorPiece = Piece::black | Piece::rook;
					}
					else if (promotionPiece == "bishop") {
						cursorPiece = Piece::black | Piece::bishop;
					}
				}
			}
		}
		else if (cursorPiece & Piece::king) {
			checkForCastle(x, y, currentBoardState);
			if (cursorPiece & Piece::white) {
				currentBoardState->setWhiteKing(x + (8 * y));
				whiteShortCastle = false;
				whiteLongCastle = false;
			}
			else {
				currentBoardState->setBlackKing(x + (8 * y));
				blackShortCastle = false;
				blackLongCastle = false;
			}
		}

		disableCastle(x, y);

		currentBoardState->movePiece(movingPieceX, movingPieceY, x, y);
		refreshSquare(x, y);
		SDL_RenderPresent(Window::m_Renderer);

		checkForCheckmate();

		if (turn == 64) {
			turn = 128;
			oppositeColor = 64;
		}
		else if (turn == 128) {
			turn = 64;
			oppositeColor = 128;
		}
	}
	else {
		if (cursorPiece & turn && !v.empty()) {
			std::cout << "Possible moves: ";
			for (int i : v) {
				std::cout << i << ' ';
			}
			std::cout << "\n";
		}

		refreshPiece(movingPieceX, movingPieceY);
		SDL_RenderPresent(Window::m_Renderer);
	}
	movingPieceX = -1;
	movingPieceY = -1;
}

std::vector<int> Game::pseudoLegalMoves(int x, int y, uint8_t** board) {
	std::vector<int> possibleMoves;
	int pieceType = board[x][y];
	int piecePosition = x + (8 * y);
	uint8_t ally, enemy;
	if (pieceType & Piece::white) {
		ally = Piece::white;
		enemy = Piece::black;
	}
	else if (pieceType & Piece::black) {
		ally = Piece::black;
		enemy = Piece::white;
	}
	else {
		ally = -1;
		enemy = -1;
	}

	if (pieceType & Piece::pawn && pieceType & Piece::white) { // replace with Piece::bottomPawn = black/white and Piece::upperPawn = black/white for different starting positions
		if (y == 6 && board[x][y - 2] == 0) {
			possibleMoves.push_back(piecePosition - 16);
		}
		if (y > 0 && board[x][y - 1] == 0) {
			possibleMoves.push_back(piecePosition - 8);
		}
		if (x > 0 && y > 0 && board[x - 1][y - 1] & Piece::black || (x - 1) + 8 * (y - 1) == enPassant) {
			possibleMoves.push_back(piecePosition - 9);
		}
		if (x < 7 && y > 0 && board[x + 1][y - 1] & Piece::black || (x + 1) + 8 * (y - 1) == enPassant) {
			possibleMoves.push_back(piecePosition - 7);
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::pawn && pieceType & Piece::black) { 
		if (y == 1 && board[x][y + 2] == 0) {
			possibleMoves.push_back(piecePosition + 16);
		}
		if (y < 7 && board[x][y + 1] == 0) {
			possibleMoves.push_back(piecePosition + 8);

		}
		if (x > 0 && y < 7 && board[x - 1][y + 1] & Piece::white || (x - 1) + 8 * (y + 1) == enPassant) {
			possibleMoves.push_back(piecePosition + 7);
		}
		if (y < 7 && x < 7 && board[x + 1][y + 1] & Piece::white || (x + 1) + 8 * (y + 1) == enPassant) {
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
			if (whiteShortCastle) {
				if (board[5][7] == 0 && board[6][7] == 0) {
					possibleMoves.push_back(x + 2 + (8 * y));
				}
			}
			if (whiteLongCastle) {
				if (board[3][7] == 0 && board[2][7] == 0 && board[1][7] == 0) {
					possibleMoves.push_back(x - 2 + (8 * y));
				}
			}
		}
		else if (pieceType & Piece::black) {
			if (blackShortCastle) {
				if (board[5][0] == 0 && board[6][0] == 0) {
					possibleMoves.push_back(x + 2 + (8 * y));
				}
			}
			if (blackLongCastle) {
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

std::vector<int> Game::LegalMoves(int PieceX, int PieceY, std::vector<int> vec) {
	std::vector<int> legalMoves;
	uint8_t enemy = -1;
	bool isLegal;

	if (boardState->getBoard()[PieceX][PieceY] & Piece::white) {
		enemy = Piece::black;
	}
	else if (boardState->getBoard()[PieceX][PieceY] & Piece::black) {
		enemy = Piece::white;
	}

	for (int move1 : vec) {
		isLegal = true;
		BoardState* boardStatecopy = new BoardState();

		uint8_t** board = new uint8_t * [Xboxes];
		for (int i = 0; i < Xboxes; ++i) {
			board[i] = new uint8_t[Yboxes];
		}
		for (int x = 0; x < Xboxes; ++x) {
			for (int y = 0; y < Yboxes; ++y) {

				board[x][y] = uint8_t(boardState->getBoard()[x][y]);
			}
		}
		boardStatecopy->setBoard(board);
		boardStatecopy->setWhiteKing(boardState->getWhiteKing());
		boardStatecopy->setBlackKing(boardState->getBlackKing());

		int destinationX = move1 % 8;
		int destinationY = move1 / 8;
		vec.pop_back();

		boardStatecopy->movePiece(PieceX, PieceY, destinationX, destinationY);

		if (boardStatecopy->getBoard()[destinationX][destinationY] & Piece::king) {
			if (boardStatecopy->getBoard()[destinationX][destinationY] & Piece::white) {
				boardStatecopy->setWhiteKing(destinationY + (8 * destinationY));
				checkForCastle(destinationX, destinationY, boardStatecopy);
				/*
				if (checkForCheck(destinationX, destinationY, boardStatecopy)) {
					std::cout << "Check: " << move1 << '\n';
					isLegal = false;
					break;
				}
				*/
			}
			else if (boardStatecopy->getBoard()[destinationX][destinationY] & Piece::black) {
				boardStatecopy->setBlackKing(destinationY + (8 * destinationY));
				checkForCastle(destinationX, destinationY, boardStatecopy);
				/*
				if (checkForCheck(destinationX, destinationY, boardStatecopy)) {
					std::cout << "Check: " << move1 << '\n';
					isLegal = false;
					break;
				}
				*/
			}
		}

		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (boardStatecopy->getBoard()[j][i] & enemy) {
					std::vector<int> v = pseudoLegalMoves(j, i, boardStatecopy->getBoard());

					if (boardStatecopy->getBoard()[destinationX][destinationY] & Piece::white) {
						if (std::count(v.begin(), v.end(), boardStatecopy->getWhiteKing())) {
							isLegal = false;
						}
					}
					if (boardStatecopy->getBoard()[destinationX][destinationY] & Piece::black) {
						if (std::count(v.begin(), v.end(), boardStatecopy->getBlackKing())) {
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

void Game::checkForCastle(int x, int y, BoardState* board) {
	if (whiteShortCastle == true && x + (8 * y) == 62) {
		board->movePiece(7, 7, 5, 7);
		refreshTile(7, 7);
		refreshPiece(5, 7);
	}
	else if (whiteLongCastle == true && x + (8 * y) == 58) {
		board->movePiece(0, 7, 3, 7);
		refreshTile(0, 7);
		refreshPiece(3, 7);
	}
	else if (blackShortCastle == true && x + (8 * y) == 6) {
		board->movePiece(7, 0, 5, 0);
		refreshTile(7, 0);
		refreshPiece(5, 0);
	}
	else if (whiteLongCastle == true && x + (8 * y) == 2) {
		board->movePiece(0, 0, 3, 0);
		refreshTile(0, 0);
		refreshPiece(3, 0);
	}
}

void Game::disableCastle(int x, int y) {
	if (whiteShortCastle) {
		if (x == 7 && y == 7 || movingPieceX == 7 && movingPieceY == 7) {
			whiteShortCastle = false;
		}
	}
	if (whiteLongCastle) {
		if (x == 0 && y == 7 || movingPieceX == 0 && movingPieceY == 7) {
			whiteLongCastle = false;
		}
	}
	if (blackShortCastle) {
		if (x == 7 && y == 0 || movingPieceX == 7 && movingPieceY == 0) {
			blackShortCastle = false;
		}
	}
	if (blackLongCastle) {
		if (x == 0 && y == 0 || movingPieceX == 0 && movingPieceY == 0) {
			blackLongCastle = false;
		}
	}
}

bool Game::checkForCheck(int PieceX, int PieceY, BoardState* board) {
	std::vector<int> alliedPieces;
	bool isCheck = false;
	bool moveIsGood = false;
	int positionX, positionY;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (board->getBoard()[j][i] & oppositeColor) {
				std::vector<int> v = LegalMoves(j, i, pseudoLegalMoves(j, i, board->getBoard()));
				if (turn & Piece::white) {
					if (std::count(v.begin(), v.end(), board->getWhiteKing())) {
						isCheck = true;
					}
				}
				else if (turn & Piece::black) {
					if (std::count(v.begin(), v.end(), board->getBlackKing())) {
						isCheck = true;
					}
				}
			}
			else if (board->getBoard()[j][i] & turn) {
				alliedPieces.push_back(j + (8 * i));
			}
		}
	}
	if (isCheck) {
		for (int position : alliedPieces) {
			positionX = position % 8;
			positionY = position / 8;
			std::vector<int> v = LegalMoves(positionX, positionY, pseudoLegalMoves(positionX, positionY, board->getBoard()));
			if (PieceX + (8 * PieceY) == position) {
				if (!v.empty()) {
					moveIsGood = true;
				}
			}
		}
		if (!moveIsGood) {
			std::cout << "Move isn't good\n";
			return false;
		}
	}
	return true;
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
				std::vector<int> v = pseudoLegalMoves(j, i, boardState->getBoard());
				if (turn & Piece::white) {
					if (std::count(v.begin(), v.end(), boardState->getBlackKing())) {
						attackingPieces.push_back(j + (8 * i));
						isCheck = true;
					}
				}
				else if (turn & Piece::black) {
					if (std::count(v.begin(), v.end(), boardState->getWhiteKing())) {
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
			std::vector<int> v = LegalMoves(positionX, positionY, pseudoLegalMoves(positionX, positionY, boardState->getBoard()));
			if (!v.empty()) {
				std::cout << "not empty!\n";
				for (int i : v) {
					std::cout << i << '\n';
				}
				counter += 1;
			}
		}
		std::cout << "Check " << counter << '\n';
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

//check for checkmate after a move (doesn't work)
//checkmate detection doesn't work correctly (rook slider couldn't be taken by a queen, king was able to move to a threatened square)
//replace dragging with clicking
//ability to play as white or black
//repetition
//halfmoves
//fullmoves