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
			setWhiteKing(y2 * 8 + x2);
			setWhiteShortCastle(false);
			setWhiteLongCastle(false);
		}
		else
		{
			setBlackKing(y2 * 8 + x2);
			setBlackShortCastle(false);
			setBlackLongCastle(false);
		}
	}

	disableCastle(x1, y1, x2, y2);

	std::swap(currentTurn, oppositeTurn);
}

std::unordered_set<int> BoardState::calculatePseudoLegalMoves(int x, int y)
{
	std::unordered_set<int> possibleMoves;

	int pieceType = getPiece(x, y);
	int piecePosition = x + (8 * y);
	uint8_t enemy = (pieceType & Piece::white) ? Piece::black : Piece::white;

	// replace with Piece::bottomPawn = black/white and Piece::upperPawn = black/white for different starting positions
	if (pieceType & Piece::pawn && pieceType & Piece::white)
	{
		if (y == 6 && board[y - 1][x] == 0 && board[y - 2][x] == 0)
		{
			possibleMoves.insert(piecePosition - 16);
		}
		if (y > 0 && board[y - 1][x] == 0)
		{
			possibleMoves.insert(piecePosition - 8);
		}
		if (x > 0 && y > 0 && (board[y - 1][x - 1] & Piece::black || (x - 1) + 8 * (y - 1) == getEnPassant()))
		{
			possibleMoves.insert(piecePosition - 9);
		}
		if (x < 7 && y > 0 && (board[y - 1][x + 1] & Piece::black || (x + 1) + 8 * (y - 1) == getEnPassant()))
		{
			possibleMoves.insert(piecePosition - 7);
		}
		return possibleMoves;
	}
	else if (pieceType & Piece::pawn && pieceType & Piece::black)
	{
		if (y == 1 && board[y + 1][x] == 0 && board[y + 2][x] == 0)
		{
			possibleMoves.insert(piecePosition + 16);
		}
		if (y < 7 && board[y + 1][x] == 0)
		{
			possibleMoves.insert(piecePosition + 8);

		}
		if (x > 0 && y < 7 && (board[y + 1][x - 1] & Piece::white || (x - 1) + 8 * (y + 1) == getEnPassant()))
		{
			possibleMoves.insert(piecePosition + 7);
		}
		if (y < 7 && x < 7 && (board[y + 1][x + 1] & Piece::white || (x + 1) + 8 * (y + 1) == getEnPassant()))
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
						if (board[y + (k * directionY)][x + (k * directionX)] & enemy)
						{
							b = false;
							if (!(std::count(possibleMoves.begin(), possibleMoves.end(), x + (k * directionX) + (8 * (y + (k * directionY))))))
							{
								possibleMoves.insert(x + (k * directionX) + (8 * (y + (k * directionY))));
							}
							break;
						}
						if (board[y + (k * directionY)][x + (k * directionX)] != 0)
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
			if (board[y - 1][x - 2] == 0 || board[y - 1][x - 2] & enemy)
			{
				possibleMoves.insert(x - 2 + (8 * (y - 1)));
			}
		}
		if (x - 1 > -1 && y - 2 > -1)
		{
			if (board[y - 2][x - 1] == 0 || board[y - 2][x - 1] & enemy)
			{
				possibleMoves.insert(x - 1 + (8 * (y - 2)));
			}
		}
		if (!(x + 1 > 7) && y - 2 > -1)
		{
			if (board[y - 2][x + 1] == 0 || board[y - 2][x + 1] & enemy)
			{
				possibleMoves.insert(x + 1 + (8 * (y - 2)));
			}
		}
		if (!(x + 2 > 7) && y - 1 > -1)
		{
			if (board[y - 1][x + 2] == 0 || board[y - 1][x + 2] & enemy)
			{
				possibleMoves.insert(x + 2 + (8 * (y - 1)));
			}
		}
		if (!(x + 2 > 7) && !(y + 1 > 7))
		{
			if (board[y + 1][x + 2] == 0 || board[y + 1][x + 2] & enemy)
			{
				possibleMoves.insert(x + 2 + (8 * (y + 1)));
			}
		}
		if (!(x + 1 > 7) && !(y + 2 > 7))
		{
			if (board[y + 2][x + 1] == 0 || board[y + 2][x + 1] & enemy)
			{
				possibleMoves.insert(x + 1 + (8 * (y + 2)));
			}
		}
		if (x - 1 > -1 && !(y + 2 > 7))
		{
			if (board[y + 2][x - 1] == 0 || board[y + 2][x - 1] & enemy)
			{
				possibleMoves.insert(x - 1 + (8 * (y + 2)));
			}
		}
		if (x - 2 > -1 && !(y + 1 > 7))
		{
			if (board[y + 1][x - 2] == 0 || board[y + 1][x - 2] & enemy)
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
						if (board[y + (k * directionY)][x + (k * directionX)] & enemy)
						{
							b = false;
							if (!(std::count(possibleMoves.begin(), possibleMoves.end(), x + (k * directionX) + (8 * (y + (k * directionY))))))
							{
								possibleMoves.insert(x + (k * directionX) + (8 * (y + (k * directionY))));
							}
							break;
						}
						if (board[y + (k * directionY)][x + (k * directionX)] != 0)
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
						if (board[y + (k * directionY)][x + (k * directionX)] & enemy)
						{
							b = false;
							if (!(std::count(possibleMoves.begin(), possibleMoves.end(), x + (k * directionX) + (8 * (y + (k * directionY))))))
							{
								possibleMoves.insert(x + (k * directionX) + (8 * (y + (k * directionY))));
							}
							break;
						}
						if (board[y + (k * directionY)][x + (k * directionX)] != 0)
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
						if (board[y + (k * directionY)][x + (k * directionX)] & enemy)
						{
							b = false;
							if (!(std::count(possibleMoves.begin(), possibleMoves.end(), x + (k * directionX) + (8 * (y + (k * directionY))))))
							{
								possibleMoves.insert(x + (k * directionX) + (8 * (y + (k * directionY))));
							}
							break;
						}
						if (board[y + (k * directionY)][x + (k * directionX)] != 0)
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
			if (board[y - 1][x - 1] == 0 || board[y - 1][x - 1] & enemy)
			{
				possibleMoves.insert(x - 1 + (8 * (y - 1)));
			}
		}
		if (y - 1 > -1)
		{
			if (board[y - 1][x] == 0 || board[y - 1][x] & enemy)
			{
				possibleMoves.insert(x + (8 * (y - 1)));
			}
		}
		if (!(x + 1 > 7) && y - 1 > -1)
		{
			if (board[y - 1][x + 1] == 0 || board[y - 1][x + 1] & enemy)
			{
				possibleMoves.insert(x + 1 + (8 * (y - 1)));
			}
		}
		if (!(x + 1 > 7))
		{
			if (board[y][x + 1] == 0 || board[y][x + 1] & enemy)
			{
				possibleMoves.insert(x + 1 + (8 * y));
			}
		}
		if (!(x + 1 > 7) && !(y + 1 > 7))
		{
			if (board[y + 1][x + 1] == 0 || board[y + 1][x + 1] & enemy)
			{
				possibleMoves.insert(x + 1 + (8 * (y + 1)));
			}
		}
		if (!(y + 1 > 7))
		{
			if (board[y + 1][x] == 0 || board[y + 1][x] & enemy)
			{
				possibleMoves.insert(x + (8 * (y + 1)));
			}
		}
		if (x - 1 > -1 && !(y + 1 > 7))
		{
			if (board[y + 1][x - 1] == 0 || board[y + 1][x - 1] & enemy)
			{
				possibleMoves.insert(x - 1 + (8 * (y + 1)));
			}
		}
		if (x - 1 > -1)
		{
			if (board[y][x - 1] == 0 || board[y][x - 1] & enemy)
			{
				possibleMoves.insert(x - 1 + (8 * y));
			}
		}

		if (pieceType & Piece::white)
		{
			if (getWhiteShortCastle())
			{
				if (board[7][5] == 0 && board[7][6] == 0)
				{
					possibleMoves.insert(x + 2 + (8 * y));
				}
			}
			if (getWhiteLongCastle())
			{
				if (board[7][3] == 0 && board[7][2] == 0 && board[7][1] == 0)
				{
					possibleMoves.insert(x - 2 + (8 * y));
				}
			}
		}
		else if (pieceType & Piece::black)
		{
			if (getBlackShortCastle())
			{
				if (board[0][5] == 0 && board[0][6] == 0)
				{
					possibleMoves.insert(x + 2 + (8 * y));
				}
			}
			if (getBlackLongCastle())
			{
				if (board[0][3] == 0 && board[0][2] == 0 && board[0][1] == 0)
				{
					possibleMoves.insert(x - 2 + (8 * y));
				}
			}
		}

		return possibleMoves;
	}

	return possibleMoves;
}

std::unordered_set<int> BoardState::calculateLegalMoves(int PieceX, int PieceY)
{
	std::unordered_set<int> pseudoLegalMoves = calculatePseudoLegalMoves(PieceX, PieceY);
	std::unordered_set<int> validLegalMoves;

	uint8_t currentPiece = getPiece(PieceX, PieceY);

	// use color mask and get the opposite
	uint8_t enemy = (currentPiece & Piece::colorMask) ^ Piece::colorMask;

	if (currentPiece & Piece::king)
	{
		if (checkForCheck())
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

		BoardState boardStateCopy(*this);

		int destinationX = move % 8;
		int destinationY = move / 8;

		boardStateCopy.movePiece(PieceX, PieceY, destinationX, destinationY);

		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 8; ++x)
			{
				if (boardStateCopy.getPiece(x, y) & enemy)
				{
					std::unordered_set<int> attackedSquares = boardStateCopy.calculatePseudoLegalMoves(x, y);

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

void BoardState::checkForSpecialPawnMoves(int StartY, int FinishX, int FinishY)
{
	uint8_t currentPiece = getPiece(FinishX, FinishY);

	if (currentPiece & Piece::white && FinishX + (8 * FinishY) == getEnPassant())
	{
		getBoard()[FinishY + 1][FinishX] = 0;
	}
	else if (currentPiece & Piece::black && FinishX + (8 * FinishY) == getEnPassant())
	{
		getBoard()[FinishY - 1][FinishX] = 0;
	}

	if (currentPiece & Piece::white && FinishY == 4 && StartY == 6)
	{
		setEnPassant(FinishX + (8 * (FinishY + 1)));
	}
	else if (currentPiece & Piece::white && Piece::black && FinishY == 3 && StartY == 1)
	{
		setEnPassant(FinishX + (8 * (FinishY - 1)));
	}
	else
	{
		setEnPassant(-1);
	}

	if (((currentPiece & Piece::white) && FinishY == 0) || ((currentPiece & Piece::black) && FinishY == 7))
	{
		//if (boardState == &currentBoardState)
		//{
		//	std::cout << "Choose your piece (queen/knight/rook/bishop): ";
		//	std::string promotionPiece;
		//	std::cin >> promotionPiece;

		//	uint8_t color = currentPiece & Piece::colorMask;

		//	if (promotionPiece == "queen")
		//	{
		//		currentPiece = color | Piece::queen;
		//	}
		//	else if (promotionPiece == "knight")
		//	{
		//		currentPiece = color | Piece::knight;
		//	}
		//	else if (promotionPiece == "rook")
		//	{
		//		currentPiece = color | Piece::rook;
		//	}
		//	else if (promotionPiece == "bishop")
		//	{
		//		currentPiece = color | Piece::bishop;
		//	}
		//}
	}
}

void BoardState::checkForCastle(int x, int y)
{
	if (getWhiteShortCastle() && (x == 6 && y == 7))
	{
		movePiece(7, 7, 5, 7);
	}
	else if (getWhiteLongCastle() && (x == 2 && y == 7))
	{
		movePiece(0, 7, 3, 7);
	}
	else if (getBlackShortCastle() && (x == 6 && y == 0))
	{
		movePiece(7, 0, 5, 0);
	}
	else if (getBlackLongCastle() && (x == 2 && y == 0))
	{
		movePiece(0, 0, 3, 0);
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

bool BoardState::checkForCheck()
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			uint8_t currentPiece = getPiece(x, y);

			if (currentPiece & getOppositeTurn() && !(currentPiece & Piece::king))
			{
				std::unordered_set<int> attackedTiles = calculatePseudoLegalMoves(x, y);
				if (getCurrentTurn() & Piece::white && attackedTiles.count(getWhiteKing()) || getCurrentTurn() & Piece::black && attackedTiles.count(getBlackKing()))
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
			uint8_t currentPiece = getBoard()[y][x];

			if (currentPiece & getCurrentTurn())
			{
				auto moves = calculateLegalMoves(x, y);

				if (!moves.empty())
				{
					return false;
				}
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