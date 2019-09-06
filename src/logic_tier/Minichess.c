/*
 * Minichess.c
 *
 *  Created on: Apr 24, 2018
 *      Author: kenshiro
 */

#include "Minichess.h"

typedef struct miniChess
{
	ChessPiece chessPieceArray[MINICHESS_NUMBER_OF_PIECES * MINICHESS_NUMBER_OF_PLAYERS];
	ChessPiece *chessboard[MINICHESS_NUMBER_OF_COLUMNS][MINICHESS_NUMBER_OF_ROWS];
	int turnNumber;
	MinichessGameState gameState;
	bool isChessboardInverted;
} Minichess;

static void approachTargetSquare(int *currentColumn, int *currentRow, int targetColumn, int targetRow)
{
	if (targetColumn > *currentColumn)
		(*currentColumn)++;
	else if (targetColumn < *currentColumn)
		(*currentColumn)--;

	if (targetRow > *currentRow)
		(*currentRow)++;
	else if (targetRow < *currentRow)
		(*currentRow)--;
}

static bool isPathFree(Minichess *myMinichess, ChessPiece *myChessPiece, int targetColumn, int targetRow)
{
    bool returnValue = true;

    ChessPiece *targetSquare = myMinichess->chessboard[targetColumn][targetRow];

    if ((targetSquare!=NULL) && (myChessPiece->player==targetSquare->player))
    	returnValue = false;

    if (returnValue)
    {
	    int currentColumn = myChessPiece->column;
	    int currentRow = myChessPiece->row;

	    approachTargetSquare(&currentColumn, &currentRow, targetColumn, targetRow);

	    while (((currentColumn!=targetColumn) || (currentRow!=targetRow)) && (returnValue))
	        if (myMinichess->chessboard[currentColumn][currentRow]!=NULL)
		        returnValue = false;
		    else
		    	approachTargetSquare(&currentColumn, &currentRow, targetColumn, targetRow);
    }

    return returnValue;
}

static bool isTowerMovement(Minichess *myMinichess, ChessPiece *myChessPiece, int targetColumn, int targetRow)
{
    bool returnValue = false;

    if (((myChessPiece->pieceName==MINICHESS_TOWER) || (myChessPiece->pieceName==MINICHESS_QUEEN)) &&
		((myChessPiece->column==targetColumn) || (myChessPiece->row==targetRow)) &&
	    (isPathFree(myMinichess, myChessPiece, targetColumn, targetRow)))

	    returnValue = true;

    return returnValue;
}

static bool isKnightMovement(Minichess *myMinichess, ChessPiece *myChessPiece, int targetColumn, int targetRow)
{
    bool returnValue = false;

    ChessPiece *targetSquare = myMinichess->chessboard[targetColumn][targetRow];

    if ((myChessPiece->pieceName==MINICHESS_KNIGHT) &&
        ((targetSquare==NULL) || (myChessPiece->player!=targetSquare->player)) &&
        (((targetColumn==myChessPiece->column - 1) && (targetRow==myChessPiece->row - 2)) ||
	     ((targetColumn==myChessPiece->column - 1) && (targetRow==myChessPiece->row + 2)) ||
		 ((targetColumn==myChessPiece->column + 1) && (targetRow==myChessPiece->row - 2)) ||
		 ((targetColumn==myChessPiece->column + 1) && (targetRow==myChessPiece->row + 2)) ||
		 ((targetColumn==myChessPiece->column - 2) && (targetRow==myChessPiece->row - 1)) ||
		 ((targetColumn==myChessPiece->column - 2) && (targetRow==myChessPiece->row + 1)) ||
		 ((targetColumn==myChessPiece->column + 2) && (targetRow==myChessPiece->row - 1)) ||
		 ((targetColumn==myChessPiece->column + 2) && (targetRow==myChessPiece->row + 1))))

        returnValue = true;

    return returnValue;
}

static bool isBishopMovement(Minichess *myMinichess, ChessPiece *myChessPiece, int targetColumn, int targetRow)
{
    bool returnValue = false;

    int columnDifference = abs(myChessPiece->column - targetColumn);
    int rowDifference = abs(myChessPiece->row - targetRow);

    if (((myChessPiece->pieceName==MINICHESS_BISHOP) || (myChessPiece->pieceName==MINICHESS_QUEEN)) &&
    	(columnDifference==rowDifference) &&
		(isPathFree(myMinichess, myChessPiece, targetColumn, targetRow)))

	    returnValue = true;

    return returnValue;
}

static bool isQueenMovement(Minichess *myMinichess, ChessPiece *myChessPiece, int targetColumn, int targetRow)
{
    bool returnValue = false;

    if ((myChessPiece->pieceName==MINICHESS_QUEEN) &&
    	((isTowerMovement(myMinichess, myChessPiece, targetColumn, targetRow)) ||
		 (isBishopMovement(myMinichess, myChessPiece, targetColumn, targetRow))))

		returnValue = true;

    return returnValue;
}

static bool isKingMovement(Minichess *myMinichess, ChessPiece *myChessPiece, int targetColumn, int targetRow)
{
    bool returnValue = false;

    int columnDifference = abs(myChessPiece->column - targetColumn);
    int rowDifference = abs(myChessPiece->row - targetRow);

    if ((myChessPiece->pieceName==MINICHESS_KING) &&
    	(columnDifference<=1) && (rowDifference<=1) &&
		(isPathFree(myMinichess, myChessPiece, targetColumn, targetRow)))

	    returnValue = true;

    return returnValue;
}

static bool isPawnMovement(Minichess *myMinichess, ChessPiece *myChessPiece, int targetColumn, int targetRow)
{
    bool returnValue = false;

    ChessPiece *targetSquare = myMinichess->chessboard[targetColumn][targetRow];

    bool isVerticalMovement = (targetColumn == myChessPiece->column);
    bool isAdjacentColumn = (targetColumn == myChessPiece->column - 1) || (targetColumn == myChessPiece->column + 1);

    bool isAscendingMovement = ((targetRow == myChessPiece->row + 1) && !(myMinichess->isChessboardInverted)) ||
                               ((targetRow == myChessPiece->row - 1) && (myMinichess->isChessboardInverted));

    bool isDescendingMovement = ((targetRow == myChessPiece->row - 1) && !(myMinichess->isChessboardInverted)) ||
            					((targetRow == myChessPiece->row + 1) && (myMinichess->isChessboardInverted));

    bool isTargetSquareEmpty = (targetSquare == NULL);

    bool isWhitePlayer = (myChessPiece->player == MINICHESS_WHITE_PLAYER);
    bool isBlackPlayer = (myChessPiece->player == MINICHESS_BLACK_PLAYER);
    bool isTargetSquareWhitePiece = (targetSquare!=NULL) && (targetSquare->player==MINICHESS_WHITE_PLAYER);
    bool isTargetSquareBlackPiece = (targetSquare!=NULL) && (targetSquare->player==MINICHESS_BLACK_PLAYER);

    if ((myChessPiece->pieceName >= MINICHESS_PAWN_1) && (myChessPiece->pieceName < MINICHESS_NUMBER_OF_PIECES) &&
		((isWhitePlayer && isAdjacentColumn && isAscendingMovement && isTargetSquareBlackPiece) ||
	     (isWhitePlayer && isVerticalMovement && isAscendingMovement && isTargetSquareEmpty) ||
	     (isBlackPlayer && isAdjacentColumn && isDescendingMovement && isTargetSquareWhitePiece) ||
	     (isBlackPlayer && isVerticalMovement && isDescendingMovement && isTargetSquareEmpty)))

    	returnValue = true;

    return returnValue;
}

static bool isLegalMovement(Minichess *myMinichess, ChessPiece *myChessPiece, int targetColumn, int targetRow)
{
	bool returnValue = false;

	if ((myChessPiece->isAlive) &&
		((isTowerMovement(myMinichess, myChessPiece, targetColumn, targetRow)) ||
		 (isKnightMovement(myMinichess, myChessPiece, targetColumn, targetRow)) ||
		 (isBishopMovement(myMinichess, myChessPiece, targetColumn, targetRow)) ||
		 (isQueenMovement(myMinichess, myChessPiece, targetColumn, targetRow)) ||
		 (isKingMovement(myMinichess, myChessPiece, targetColumn, targetRow)) ||
		 (isPawnMovement(myMinichess, myChessPiece, targetColumn, targetRow))))

		returnValue = true;

	return returnValue;
}

static bool isKingThreatened(Minichess *myMinichess, MinichessPlayer myPlayer)
{
	bool returnValue = false;

	int kingPieceIndex = MINICHESS_KING + myPlayer * MINICHESS_NUMBER_OF_PIECES;

	int kingColumn = myMinichess->chessPieceArray[kingPieceIndex].column;
	int kingRow = myMinichess->chessPieceArray[kingPieceIndex].row;

	int firstEnemyPieceIndex = (1 - myPlayer) * MINICHESS_NUMBER_OF_PIECES;
	int lastEnemyPieceIndex = firstEnemyPieceIndex + MINICHESS_NUMBER_OF_PIECES - 1;

	int i = firstEnemyPieceIndex;

	while ((i<=lastEnemyPieceIndex) && (!returnValue))
	{
		ChessPiece *enemyChessPiece = &(myMinichess->chessPieceArray[i]);

		if (isLegalMovement(myMinichess, enemyChessPiece, kingColumn, kingRow))
	        returnValue = true;

	    i++;
	}

    return returnValue;
}

static MinichessErrorCode updateChessboard (Minichess *myMinichess, ChessPiece *myChessPiece, int targetColumn, int targetRow, bool isSimulation)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	MinichessPlayer myPlayer = myChessPiece->player;

	ChessPiece *targetSquare = myMinichess->chessboard[targetColumn][targetRow];

	int sourceColumn = myChessPiece->column;
	int sourceRow = myChessPiece->row;

	myChessPiece->column = targetColumn;
	myChessPiece->row = targetRow;

	if (targetSquare!=NULL)
		targetSquare->isAlive = false;

	myMinichess->chessboard[sourceColumn][sourceRow] = NULL;
	myMinichess->chessboard[targetColumn][targetRow] = myChessPiece;

	bool kingThreatened = isKingThreatened(myMinichess, myPlayer);

	if (kingThreatened || isSimulation)
	{
		myChessPiece->column = sourceColumn;
		myChessPiece->row = sourceRow;

		if (targetSquare!=NULL)
			targetSquare->isAlive = true;

		myMinichess->chessboard[sourceColumn][sourceRow] = myChessPiece;
		myMinichess->chessboard[targetColumn][targetRow] = targetSquare;
	}

	if (kingThreatened)
		returnValue = MINICHESS_INVALID_MOVEMENT_ERROR;

	return returnValue;
}

static MinichessErrorCode movePiece (Minichess *myMinichess, ChessPiece *myChessPiece, int targetColumn, int targetRow)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	int pawnToQueenRow;

	if (myMinichess->isChessboardInverted)
		pawnToQueenRow = myChessPiece->player * (MINICHESS_NUMBER_OF_ROWS - 1);
	else
		pawnToQueenRow = (1 - myChessPiece->player) * (MINICHESS_NUMBER_OF_ROWS - 1);

	if (!isLegalMovement(myMinichess, myChessPiece, targetColumn, targetRow))
		returnValue = MINICHESS_INVALID_MOVEMENT_ERROR;

	//Update the chessboard and check that the king is not threatened
	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		returnValue = updateChessboard(myMinichess, myChessPiece, targetColumn, targetRow, false);

	//Check if a pawn is promoting
	if ((returnValue==MINICHESS_RETURN_VALUE_OK) &&
		(myChessPiece->pieceName >= MINICHESS_PAWN_1) &&
		(myChessPiece->pieceName < MINICHESS_NUMBER_OF_PIECES) &&
		(myChessPiece->row == pawnToQueenRow))

	    myChessPiece->pieceName = MINICHESS_QUEEN;

	return returnValue;
}

static bool isCheckmate(Minichess *myMinichess, MinichessPlayer myPlayer)
{
	bool returnValue = true;

	int i = 0;

	while ((i<MINICHESS_NUMBER_OF_PIECES) && returnValue)
	{
		ChessPiece *myChessPiece = &(myMinichess->chessPieceArray[i + myPlayer * MINICHESS_NUMBER_OF_PIECES]);

		int targetColumn = 0;

		while (targetColumn<MINICHESS_NUMBER_OF_COLUMNS && myChessPiece->isAlive && returnValue)
		{
			int targetRow = 0;

			while (targetRow<MINICHESS_NUMBER_OF_ROWS && returnValue)
			{
				if (isLegalMovement(myMinichess, myChessPiece, targetColumn, targetRow))
				{
					MinichessErrorCode result = updateChessboard(myMinichess, myChessPiece, targetColumn, targetRow, true);

					if (result==MINICHESS_RETURN_VALUE_OK)
						returnValue = false;
				}

				targetRow++;
			}

			targetColumn++;
		}

		i++;
	}

	return returnValue;
}

static void updateGameState(Minichess *myMinichess, MinichessPlayer myPlayer)
{
	if (myPlayer==MINICHESS_WHITE_PLAYER)
	{
		if (isCheckmate(myMinichess, MINICHESS_BLACK_PLAYER))
			myMinichess->gameState = MINICHESS_GAME_STATE_WHITE_PLAYER_WINS;
		else
			myMinichess->gameState = MINICHESS_GAME_STATE_BLACK_PLAYER_PLY;
	}
	else if (myPlayer==MINICHESS_BLACK_PLAYER)
	{
		if (isCheckmate(myMinichess, MINICHESS_WHITE_PLAYER))
		{
			myMinichess->gameState = MINICHESS_GAME_STATE_BLACK_PLAYER_WINS;
		}
		else if (myMinichess->turnNumber < MINICHESS_MAXIMUM_NUMBER_OF_TURNS)
		{
			myMinichess->turnNumber++;
			myMinichess->gameState = MINICHESS_GAME_STATE_WHITE_PLAYER_PLY;
		}
		else
			myMinichess->gameState = MINICHESS_GAME_STATE_STALEMATE;
	}
}

static void initializeMinichess(Minichess *myMinichess)
{
	myMinichess->turnNumber = 1;
	myMinichess->gameState = MINICHESS_GAME_STATE_WHITE_PLAYER_PLY;
	myMinichess->isChessboardInverted = false;

	for (int column=0; column<MINICHESS_NUMBER_OF_COLUMNS; column++)
		for (int row=0; row<MINICHESS_NUMBER_OF_ROWS; row++)
			myMinichess->chessboard[column][row] = NULL;

	for (int player=0; player < MINICHESS_NUMBER_OF_PLAYERS; player++)
		for (int pieceName=0; pieceName < MINICHESS_NUMBER_OF_PIECES; pieceName++)
		{
			int chessPieceArrayIndex = pieceName + player * MINICHESS_NUMBER_OF_PIECES;

			int column = pieceName % (MINICHESS_NUMBER_OF_PIECES / 2);
			int row = player * (MINICHESS_NUMBER_OF_ROWS - 1);

			//Adjust the row if the chess piece is a pawn
			if (pieceName >= MINICHESS_PAWN_1)
				row = row + 1 - player * 2;

			myMinichess->chessPieceArray[chessPieceArrayIndex].player = player;
			myMinichess->chessPieceArray[chessPieceArrayIndex].pieceName = pieceName;
			myMinichess->chessPieceArray[chessPieceArrayIndex].isAlive = true;
			myMinichess->chessPieceArray[chessPieceArrayIndex].column = column;
			myMinichess->chessPieceArray[chessPieceArrayIndex].row = row;

			myMinichess->chessboard[column][row] = &(myMinichess->chessPieceArray[chessPieceArrayIndex]);
		}
}

MinichessErrorCode createMinichess(Minichess **myMinichess)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	if (myMinichess==NULL)
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	//Allocate memory
	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		*myMinichess = malloc(sizeof(Minichess));

		if (*myMinichess==NULL)
			returnValue = MINICHESS_MEMORY_ALLOCATION_ERROR;
	}

	//Initialize
	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		initializeMinichess(*myMinichess);

	return returnValue;
}

MinichessErrorCode destroyMinichess(Minichess **myMinichess)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	if (myMinichess==NULL)
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		free(*myMinichess);
		*myMinichess = NULL;
	}

	return returnValue;
}

MinichessErrorCode resetMinichess(Minichess *myMinichess)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	if (myMinichess==NULL)
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		initializeMinichess(myMinichess);

	return returnValue;
}

MinichessErrorCode copyMinichess(Minichess *myMinichess, Minichess *myMinichessCopy)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	if ((myMinichess==NULL) || (myMinichessCopy==NULL))
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		int chessPieceArraySize = sizeof(myMinichess->chessPieceArray);
		int chessBoardSize = sizeof(myMinichess->chessboard);

		memcpy(myMinichessCopy->chessPieceArray, myMinichess->chessPieceArray, chessPieceArraySize);
		memcpy(myMinichessCopy->chessboard, myMinichess->chessboard, chessBoardSize);

		myMinichessCopy->gameState = myMinichess->gameState;
		myMinichessCopy->isChessboardInverted = myMinichess->isChessboardInverted;
		myMinichessCopy->turnNumber = myMinichess->turnNumber;
	}

	return returnValue;
}

MinichessErrorCode executePlayerAction(Minichess *myMinichess, PlayerAction myPlayerAction)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	if (myMinichess==NULL)
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	if ((myPlayerAction.player!=MINICHESS_WHITE_PLAYER) && (myPlayerAction.player!=MINICHESS_BLACK_PLAYER))
		returnValue = MINICHESS_INVALID_PLAYER_ERROR;

	if ((myPlayerAction.chessPieceName < 0) || (myPlayerAction.chessPieceName >= MINICHESS_NUMBER_OF_PIECES))
		returnValue = MINICHESS_INVALID_PIECE_ERROR;

	if ((myPlayerAction.targetColumn < 0) || (myPlayerAction.targetColumn >= MINICHESS_NUMBER_OF_COLUMNS) ||
		(myPlayerAction.targetRow < 0) || (myPlayerAction.targetRow >= MINICHESS_NUMBER_OF_ROWS))

		returnValue = MINICHESS_INVALID_MOVEMENT_ERROR;

	//Check the game state
	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		if ((myMinichess->gameState==MINICHESS_GAME_STATE_WHITE_PLAYER_WINS) ||
			(myMinichess->gameState==MINICHESS_GAME_STATE_BLACK_PLAYER_WINS) ||
			(myMinichess->gameState==MINICHESS_GAME_STATE_STALEMATE))

			returnValue = MINICHESS_GAME_OVER_ERROR;

		if (((myPlayerAction.player==MINICHESS_WHITE_PLAYER) && (myMinichess->gameState!=MINICHESS_GAME_STATE_WHITE_PLAYER_PLY)) ||
			((myPlayerAction.player==MINICHESS_BLACK_PLAYER) && (myMinichess->gameState!=MINICHESS_GAME_STATE_BLACK_PLAYER_PLY)))

			returnValue = MINICHESS_NOT_PLAYER_TURN_ERROR;
	}

	//Move the chess piece
	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		int chessPieceArrayIndex = myPlayerAction.chessPieceName + myPlayerAction.player * MINICHESS_NUMBER_OF_PIECES;

		ChessPiece *myChessPiece = &(myMinichess->chessPieceArray[chessPieceArrayIndex]);

		returnValue = movePiece(myMinichess, myChessPiece, myPlayerAction.targetColumn, myPlayerAction.targetRow);
	}

	//Check end-of-game conditions
	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		updateGameState(myMinichess, myPlayerAction.player);

	return returnValue;
}

MinichessErrorCode getTurnNumber(Minichess *myMinichess, int *turnNumber)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	if ((myMinichess==NULL) || (turnNumber==NULL))
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		*turnNumber = myMinichess->turnNumber;

	return returnValue;
}

MinichessErrorCode getChessboardSquare(Minichess *myMinichess, int column, int row, ChessPiece *myChessPiece)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	ChessPiece *originalChessPiece = NULL;

	if ((myMinichess==NULL) || (myChessPiece==NULL))
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	if ((column < 0) || (column >= MINICHESS_NUMBER_OF_COLUMNS) ||
		(row < 0) || (row >= MINICHESS_NUMBER_OF_ROWS))

		returnValue = MINICHESS_INVALID_BOARD_DIMENSIONS_ERROR;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		originalChessPiece = myMinichess->chessboard[column][row];

		if ((originalChessPiece!=NULL) && (!originalChessPiece->isAlive))
			returnValue = MINICHESS_INVALID_PIECE_ERROR;
	}

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		if (originalChessPiece!=NULL)
			*myChessPiece = *originalChessPiece;
		else
		{
			myChessPiece->player = MINICHESS_UNKNOWN_PLAYER;
			myChessPiece->pieceName = MINICHESS_EMPTY_SQUARE;
			myChessPiece->isAlive = false;
			myChessPiece->column = INT_MIN;
			myChessPiece->row = INT_MIN;
		}
	}

	return returnValue;
}

MinichessErrorCode getChessPiece(Minichess *myMinichess, MinichessPlayer myPlayer, MinichessPieceName myChessPieceName, ChessPiece *myChessPiece)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	if ((myMinichess==NULL) || (myChessPiece==NULL))
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	if ((myPlayer!=MINICHESS_WHITE_PLAYER) && (myPlayer!=MINICHESS_BLACK_PLAYER))
		returnValue = MINICHESS_INVALID_PLAYER_ERROR;

	if ((myChessPieceName < 0) || (myChessPieceName >= MINICHESS_NUMBER_OF_PIECES))
		returnValue = MINICHESS_INVALID_PIECE_ERROR;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		ChessPiece *originalChessPiece = &(myMinichess->chessPieceArray[myChessPieceName + myPlayer * MINICHESS_NUMBER_OF_PIECES]);

		*myChessPiece = *originalChessPiece;
	}

	return returnValue;
}

MinichessErrorCode getGameState(Minichess *myMinichess, MinichessGameState *myGameState)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	if ((myMinichess==NULL) || (myGameState==NULL))
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		*myGameState = myMinichess->gameState;

	return returnValue;
}

MinichessErrorCode invertChessboard(Minichess *myMinichess)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	if (myMinichess==NULL)
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		myMinichess->isChessboardInverted = !(myMinichess->isChessboardInverted);

		//Clean chessboard
		for (int column=0; column<MINICHESS_NUMBER_OF_COLUMNS; column++)
			for (int row=0; row<MINICHESS_NUMBER_OF_ROWS; row++)
				myMinichess->chessboard[column][row] = NULL;

		//Invert chessboard
		for (int i=0; i<MINICHESS_NUMBER_OF_PIECES_OF_ALL_PLAYERS; i++)
			if (myMinichess->chessPieceArray[i].isAlive)
			{
				int column = myMinichess->chessPieceArray[i].column;
				int oppositeRow = MINICHESS_NUMBER_OF_ROWS -1 - myMinichess->chessPieceArray[i].row;

				myMinichess->chessPieceArray[i].row = oppositeRow;

				myMinichess->chessboard[column][oppositeRow] = &(myMinichess->chessPieceArray[i]);
			}
	}

	return returnValue;
}
