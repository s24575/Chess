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
	currentTurn(other.currentTurn),
	oppositeTurn(other.oppositeTurn),
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

	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			board[y][x] = other.board[y][x];
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

void BoardState::movePiece(int x1, int y1, int x2, int y2)
{
	uint8_t currentPiece = getPiece(x1, y1);

	board[y2][x2] = currentPiece;
	board[y1][x1] = 0;

	if (currentPiece & Piece::pawn)
	{
		checkForSpecialPawnMoves(y1, x2, y2);
	}
	else if (currentPiece & Piece::king)
	{
		checkForCastle(x2, y2);
		if (currentPiece & Piece::white)
		{
			setWhiteKing({x2, y2});
			setWhiteShortCastle(false);
			setWhiteLongCastle(false);
		}
		else if (currentPiece & Piece::black)
		{
			setBlackKing({x2, y2});
			setBlackShortCastle(false);
			setBlackLongCastle(false);
		}
	}

	disableCastle(x1, y1, x2, y2);

	std::swap(currentTurn, oppositeTurn);
}

BoardState::PositionSet BoardState::calculatePseudoLegalMoves(int x, int y)
{
	PositionSet possibleMoves;
	uint8_t currentPiece = getPiece(x, y);

	auto withinBounds = [](int x, int y)
	{
		return(0 <= x && x < 8 && 0 <= y && y < 8);
	};

	auto addMove = [&](int x, int y)
	{
		if (withinBounds(x, y))
		{
			if (!(getPiece(x, y) & getCurrentTurn()))
			{
				possibleMoves.insert({ x, y });
			}
		}
	};

	auto calculateBishopMoves = [&](int x, int y)
	{
		int8_t dx[] = { -1, 1 };
		int8_t dy[] = { -1, 1 };

		for (int j = 0; j < 2; j++)
		{
			for (int i = 0; i < 2; i++)
			{
				int a = x;
				int b = y;

				while (true)
				{
					a += dx[i];
					b += dy[j];

					if (withinBounds(a, b) && !getPiece(a, b))
					{
						possibleMoves.insert({ a, b });
					}
					else
					{
						if (withinBounds(a, b) && !(getPiece(a, b) & getCurrentTurn()))
						{
							possibleMoves.insert({ a, b });
						}

						break;
					}
				}
			}
		}
	};

	auto calculateRookMoves = [&](int x, int y)
	{
		int8_t dx[] = { 0, 1, 0, -1 };
		int8_t dy[] = { 1, 0, -1, 0 };

		for (int i = 0; i < 4; i++)
		{
			int a = x;
			int b = y;

			while (true)
			{
				a += dx[i];
				b += dy[i];

				if (withinBounds(a, b) && !getPiece(a, b))
				{
					possibleMoves.insert({ a, b });
				}
				else
				{
					if (withinBounds(a, b) && !(getPiece(a, b) & getCurrentTurn()))
					{
						possibleMoves.insert({ a, b });
					}

					break;
				}
			}
		}
	};

	if (currentPiece & Piece::pawn)
	{
		if (currentPiece & Piece::black)
		{
			if (y == 6 && !getPiece(x, y - 1) && !getPiece(x, y - 2))
			{
				possibleMoves.insert({ x, y - 2 });
			}
			if (0 < y && !getPiece(x, y - 1))
			{
				possibleMoves.insert({ x, y - 1});
			}
			if (0 < x && 0 < y && (getPiece(x - 1, y - 1) & Piece::white || Position{x - 1, y - 1} == getEnPassant()))
			{
				possibleMoves.insert({x - 1, y - 1});
			}
			if (x < 7 && 0 < y && (getPiece(x + 1, y - 1) & Piece::white || Position{x + 1, y - 1} == getEnPassant()))
			{
				possibleMoves.insert({x + 1, y - 1});
			}
		}
		else if (currentPiece & Piece::white)
		{
			if (y == 1 && !getPiece(x, y + 1) && !getPiece(x, y + 2))
			{
				possibleMoves.insert({x, y + 2});
			}
			if (y < 7 && !getPiece(x, y + 1))
			{
				possibleMoves.insert({x, y + 1});

			}
			if (x > 0 && y < 7 && (getPiece(x - 1, y + 1) & Piece::black || Position{ x - 1, y + 1 } == getEnPassant()))
			{
				possibleMoves.insert({x - 1, y + 1});
			}
			if (y < 7 && x < 7 && (getPiece(x + 1, y + 1) & Piece::black || Position{ x + 1, y + 1 } == getEnPassant()))
			{
				possibleMoves.insert({x + 1, y + 1});
			}
		}
	}
	else if (currentPiece & Piece::bishop)
	{
		calculateBishopMoves(x, y);
	}
	else if (currentPiece & Piece::knight)
	{
		int8_t dx[] = { -1, 1 };
		int8_t dy[] = { -1, 1 };

		for (int j = 0; j < 2; j++)
		{
			for (int i = 0; i < 2; i++)
			{
				addMove(x + dx[i] * 2, y + dy[j] * 1);
				addMove(x + dx[i] * 1, y + dy[j] * 2);
			}
		}
	}
	else if (currentPiece & Piece::rook)
	{
		calculateRookMoves(x, y);
	}
	else if (currentPiece & Piece::queen)
	{
		calculateBishopMoves(x, y);
		calculateRookMoves(x, y);
	}
	else if (currentPiece & Piece::king)
	{
		for (int j = -1; j <= 1; j++)
		{
			for (int i = -1; i <= 1; i++)
			{
				if (i != 0 || j != 0)
				{
					addMove(x + i, y + j);
				}
			}
		}

		if (currentPiece & Piece::white)
		{
			if (getWhiteShortCastle())
			{
				if (!getPiece(5, 0) && !getPiece(6, 0))
				{
					possibleMoves.insert({ x + 2, y });
				}
			}
			if (getWhiteLongCastle())
			{
				if (!getPiece(1, 0) && !getPiece(2, 0) && !getPiece(3, 0))
				{
					possibleMoves.insert({ x - 2, y });
				}
			}
		}
		else if (currentPiece & Piece::black)
		{
			if (getBlackShortCastle())
			{
				if (!getPiece(5, 7) && !getPiece(6, 7))
				{
					possibleMoves.insert({ x + 2, y });
				}
			}
			if (getBlackLongCastle())
			{
				if (!getPiece(1, 7) && !getPiece(2, 7) && !getPiece(3, 7))
				{
					possibleMoves.insert({ x - 2, y });
				}
			}
		}
	}

	return possibleMoves;
}

int BoardState::calculateLegalMovesCount(int n)
{
	if (n == 0)
		return 1;

	int count = 0;
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if (getPiece(x, y) & getCurrentTurn())
			{
				PositionSet pseudoLegalMoves = calculatePseudoLegalMoves(x, y);

				for (Position position : pseudoLegalMoves)
				{
					BoardState boardStateCopy(*this);

					boardStateCopy.movePiece(x, y, position.first, position.second);

					if (!boardStateCopy.checkForCheck(boardStateCopy.getOppositeTurn()))
					{
						count += boardStateCopy.calculateLegalMovesCount(n - 1);
					}
				}
			}
		}
	}

	return count;
}

BoardState::PositionSet BoardState::calculateLegalMoves(int PieceX, int PieceY)
{
	PositionSet pseudoLegalMoves = calculatePseudoLegalMoves(PieceX, PieceY);
	PositionSet validLegalMoves;

	uint8_t currentPiece = getPiece(PieceX, PieceY);

	uint8_t enemy = (currentPiece & Piece::colorMask) ^ Piece::colorMask;

	if (currentPiece & Piece::king)
	{
		if (checkForCheck(getCurrentTurn()))
		{
			// disable castling
			if (currentPiece & Piece::white)
			{
				pseudoLegalMoves.erase({6, 7});
				pseudoLegalMoves.erase({2, 7});
			}
			else if (currentPiece & Piece::black)
			{
				pseudoLegalMoves.erase({6, 0});
				pseudoLegalMoves.erase({2, 0});
			}
		}
	}

	for (Position position : pseudoLegalMoves)
	{
		BoardState boardStateCopy(*this);

		boardStateCopy.movePiece(PieceX, PieceY, position.first, position.second);

		if (!boardStateCopy.checkForCheck(getCurrentTurn()))
		{
			validLegalMoves.insert(position);
		}
	}

	return validLegalMoves;
}

void BoardState::checkForSpecialPawnMoves(int StartY, int FinishX, int FinishY)
{
	uint8_t currentPiece = getPiece(FinishX, FinishY);

	if (currentPiece & Piece::white && Position{ FinishX, FinishY } == getEnPassant())
	{
		getBoard()[FinishY + 1][FinishX] = 0;
	}
	else if (currentPiece & Piece::black && Position{ FinishX, FinishY } == getEnPassant())
	{
		getBoard()[FinishY - 1][FinishX] = 0;
	}

	if (currentPiece & Piece::white && FinishY == 4 && StartY == 6)
	{
		setEnPassant({FinishX, FinishY + 1});
	}
	else if (currentPiece & Piece::black && FinishY == 3 && StartY == 1)
	{
		setEnPassant({ FinishX, FinishY - 1 });
	}
	else
	{
		setEnPassant({ -1, -1 });
	}

	if (((currentPiece & Piece::white) && FinishY == 0) || ((currentPiece & Piece::black) && FinishY == 7))
	{
		uint8_t color = currentPiece & Piece::colorMask;
		board[FinishY][FinishX] = color | Piece::queen;
	}
}

void BoardState::checkForCastle(int x, int y)
{
	if (getWhiteShortCastle() && (x == 6 && y == 0))
	{
		board[0][5] = board[0][7];
		board[0][7] = 0;
	}
	else if (getWhiteLongCastle() && (x == 2 && y == 0))
	{
		board[0][3] = board[0][0];
		board[0][0] = 0;
	}
	else if (getBlackShortCastle() && (x == 6 && y == 7))
	{
		board[7][5] = board[7][7];
		board[7][7] = 0;
	}
	else if (getBlackLongCastle() && (x == 2 && y == 7))
	{
		board[7][3] = board[7][0];
		board[7][0] = 0;
	}
}

void BoardState::disableCastle(int x1, int y1, int x2, int y2)
{
	if ((x1 == 7 && y1 == 7) || (x2 == 7 && y2 == 7))
	{
		setWhiteShortCastle(false);
	}
	else if ((x1 == 0 && y1 == 7) || (x2 == 0 && y2 == 7))
	{
		setWhiteLongCastle(false);
	}
	else if ((x1 == 7 && y1 == 0) || (x2 == 7 && y2 == 0))
	{
		setBlackShortCastle(false);
	}
	else if ((x1 == 0 && y1 == 0) || (x2 == 0 && y2 == 0))
	{
		setBlackLongCastle(false);
	}
}

bool BoardState::checkForCheck(uint8_t kingColor)
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			uint8_t currentPiece = getPiece(x, y);

			if (!(currentPiece & kingColor))
			{
				PositionSet attackedTiles = calculatePseudoLegalMoves(x, y);

				//if (kingColor & Piece::white && attackedTiles.count(getWhiteKing()) || kingColor & Piece::black && attackedTiles.count(getBlackKing()))
				//{
				//	return true;
				//}
				Position oppositeKing = (kingColor & Piece::white) ? getWhiteKing() : getBlackKing();
				if (attackedTiles.count(oppositeKing))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool BoardState::checkForCheckmate()
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if (getPiece(x, y) & getCurrentTurn() && !calculateLegalMoves(x, y).empty())
			{
				return false;
			}
		}
	}

	return true;
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
					case 'P':
						board[y][x] = Piece::white | Piece::pawn;
						break;
					case 'p':
						board[y][x] = Piece::black | Piece::pawn;
						break;
					case 'R':
						board[y][x] = Piece::white | Piece::rook;
						break;
					case 'r':
						board[y][x] = Piece::black | Piece::rook;
						break;
					case 'B':
						board[y][x] = Piece::white | Piece::bishop;
						break;
					case 'b':
						board[y][x] = Piece::black | Piece::bishop;
						break;
					case 'N':
						board[y][x] = Piece::white | Piece::knight;
						break;
					case 'n':
						board[y][x] = Piece::black | Piece::knight;
						break;
					case 'K':
						board[y][x] = Piece::white | Piece::king;
						setWhiteKing({x, y});
						break;
					case 'k':
						board[y][x] = Piece::black | Piece::king;
						setBlackKing({x, y});
						break;
					case 'Q':
						board[y][x] = Piece::white | Piece::queen;
						break;
					case 'q':
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
		setEnPassant({x, y});
	}
	i++;
}