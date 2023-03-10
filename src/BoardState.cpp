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
	board = new uint8_t*[8];
	for (int i = 0; i < 8; i++)
	{
		board[i] = new uint8_t[8];
	}

	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			board[y][x] = other.getPiece(x, y);
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
	board[FinishY][FinishX] = board[StartY][StartX];
	board[StartY][StartX] = 0;

	std::swap(currentTurn, oppositeTurn);
}

void BoardState::loadFEN(const std::string& FEN){
	int i = 0;

	for (int y = 7; y >= 0; y--)
	{
		int x = 0;
		while (FEN[i] != '/' && FEN[i] != ' ')
		{
			if (isdigit(FEN[i]))
			{
				x += FEN[i] - '0';
				i++;
			}
			else
			{
				switch (FEN[i])
				{
					case 'p':
						board[y][x] = Piece::white | Piece::pawn;
						break;
					case 'P':
						board[y][x] = Piece::black | Piece::pawn;
						break;
					case 'r':
						board[y][x] = Piece::white | Piece::rook;
						break;
					case 'R':
						board[y][x] = Piece::black | Piece::rook;
						break;
					case 'b':
						board[y][x] = Piece::white | Piece::bishop;
						break;
					case 'B':
						board[y][x] = Piece::black | Piece::bishop;
						break;
					case 'n':
						board[y][x] = Piece::white | Piece::knight;
						break;
					case 'N':
						board[y][x] = Piece::black | Piece::knight;
						break;
					case 'k':
						board[y][x] = Piece::white | Piece::king;
						setWhiteKing(x + (8 * y));
						break;
					case 'K':
						board[y][x] = Piece::black | Piece::king;
						setBlackKing(x + (8 * y));
						break;
					case 'q':
						board[y][x] = Piece::white | Piece::queen;
						break;
					case 'Q':
						board[y][x] = Piece::black | Piece::queen;
						break;
				}
				i++;
				x++;
			}
		}
		i++;
	}

	if (FEN[i] == 'w')
	{
		currentTurn = Piece::white;
		oppositeTurn = Piece::black;
	}
	else if (FEN[i] == 'b')
	{
		currentTurn = Piece::black;
		oppositeTurn = Piece::white;
	}
	i += 2;

	setWhiteShortCastle(false);
	setWhiteLongCastle(false);
	setBlackShortCastle(false);
	setBlackLongCastle(false);

	while (FEN[i] != ' ')
	{
		switch (FEN[i])
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
		i++;
	}
	i++;

	if (FEN[i] != '-')
	{
		int x = int(FEN[i]) - 'a';
		i++;
		int y = int(FEN[i]) - '1';
		setEnPassant(x + (8 * y));
	}
	i++;
}