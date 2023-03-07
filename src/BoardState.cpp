#include "BoardState.h"
#include "Piece.h"

BoardState::BoardState()
{
	board = new uint8_t * [8];
	for (int i = 0; i < 8; ++i)
	{
		board[i] = new uint8_t[8]{ 0 };
	}
}

BoardState::BoardState(const BoardState& other) :
	enPassant(other.enPassant),
	whiteKing(other.whiteKing),
	blackKing(other.blackKing),
	whiteShortCastle(other.whiteShortCastle),
	whiteLongCastle(other.whiteLongCastle),
	blackShortCastle(other.blackShortCastle),
	blackLongCastle(other.blackLongCastle)
{
	board = new uint8_t * [8];
	for (int i = 0; i < 8; ++i)
	{
		board[i] = new uint8_t[8];
	}
	uint8_t** otherBoard = other.getBoard();
	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			board[y][x] = otherBoard[y][x];
		}
	}
}

BoardState::~BoardState()
{
	for (int x = 0; x < 8; x++)
	{
		delete[] board[x];
	}
	delete[] board;
}

void BoardState::movePiece(int StartX, int StartY, int FinishX, int FinishY)
{
	board[FinishX][FinishY] = board[StartX][StartY];
	board[StartX][StartY] = 0;

	std::swap(currentTurn, oppositeTurn);
}

void BoardState::loadFEN(const std::string& FEN){
	int index = 0;
	int x;

	//for (int y = 0; y < 8; ++y) {// -> black at the bottom
	for (int y = 7; y > -1; --y)
	{
		x = 0;
		while (FEN[index] != '/' && FEN[index] != ' ')
		{
			if (isdigit(FEN[index]))
			{
				x += FEN[index] - '0';
				index++;
			}
			else
			{
				switch (FEN[index])
				{
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
						setWhiteKing(x + (8 * y));
						break;
					case 'K':
						board[x][y] = Piece::black | Piece::king;
						setBlackKing(x + (8 * y));
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

	if (FEN[index] == 'w')
	{
		currentTurn = Piece::white;
		oppositeTurn = Piece::black;
	}
	else if (FEN[index] == 'b')
	{
		currentTurn = Piece::black;
		oppositeTurn = Piece::white;
	}
	index += 2;

	setWhiteShortCastle(false);
	setWhiteLongCastle(false);
	setBlackShortCastle(false);
	setBlackLongCastle(false);

	while (FEN[index] != ' ')
	{
		switch (FEN[index])
		{
			case 'K':
				setWhiteShortCastle(true);
				break;
			case 'Q':
				setWhiteLongCastle(true);
				break;
			case 'k':
				setBlackShortCastle(true);
				break;
			case 'q':
				setBlackLongCastle(true);
				break;
		}
		index++;
	}
	index++;

	if (FEN[index] != '-')
	{
		int x, y;
		x = int(FEN[index]) - 'a';
		index++;
		y = int(FEN[index]) - '1';
		setEnPassant(x + (8 * y));
	}
	index++;
}