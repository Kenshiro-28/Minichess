/*
 * ConsoleManager.c
 *
 *  Created on: Apr 25, 2018
 *      Author: kenshiro
 */

#include "ConsoleManager.h"

#define CHESS_PIECE_TAG_SIZE 3

#define EMPTY_SQUARE_TAG "   "

#define WHITE_TOWER_TAG " T "
#define WHITE_KNIGHT_TAG " K "
#define WHITE_BISHOP_TAG " B "
#define WHITE_QUEEN_TAG " Q "
#define WHITE_KING_TAG " L "
#define WHITE_PAWN_TAG " P "
#define BLACK_TOWER_TAG "..T"
#define BLACK_KNIGHT_TAG "..K"
#define BLACK_BISHOP_TAG "..B"
#define BLACK_QUEEN_TAG "..Q"
#define BLACK_KING_TAG "..L"
#define BLACK_PAWN_TAG "..P"

#define WHITE_PLAYER_PLY_TAG "White player ply"
#define BLACK_PLAYER_PLY_TAG "Black player ply"
#define WHITE_PLAYER_WINS_TAG "White player wins"
#define BLACK_PLAYER_WINS_TAG "Black player wins"
#define STALEMATE_TAG "Stalemate"

static void getChessPieceTag(MinichessPieceName myChessPieceName, MinichessPlayer myPlayer, char *myChessPieceTag)
{
	switch (myChessPieceName)
	{
	    case MINICHESS_TOWER:

	    	if (myPlayer==MINICHESS_WHITE_PLAYER)
	    		strcpy(myChessPieceTag, WHITE_TOWER_TAG);
	    	else
	    		strcpy(myChessPieceTag, BLACK_TOWER_TAG);

	    	break;

		case MINICHESS_KNIGHT:

		   if (myPlayer==MINICHESS_WHITE_PLAYER)
			   strcpy(myChessPieceTag, WHITE_KNIGHT_TAG);
		   else
			   strcpy(myChessPieceTag, BLACK_KNIGHT_TAG);

		   break;

		case MINICHESS_BISHOP:

		   if (myPlayer==MINICHESS_WHITE_PLAYER)
			   strcpy(myChessPieceTag, WHITE_BISHOP_TAG);
		   else
			   strcpy(myChessPieceTag, BLACK_BISHOP_TAG);

		   break;

		case MINICHESS_QUEEN:

			if (myPlayer==MINICHESS_WHITE_PLAYER)
				strcpy(myChessPieceTag, WHITE_QUEEN_TAG);
			else
				strcpy(myChessPieceTag, BLACK_QUEEN_TAG);

			break;

		case MINICHESS_KING:

			if (myPlayer==MINICHESS_WHITE_PLAYER)
				strcpy(myChessPieceTag, WHITE_KING_TAG);
			else
				strcpy(myChessPieceTag, BLACK_KING_TAG);

			break;

		case MINICHESS_PAWN_1:
		case MINICHESS_PAWN_2:
		case MINICHESS_PAWN_3:
		case MINICHESS_PAWN_4:
		case MINICHESS_PAWN_5:

			if (myPlayer==MINICHESS_WHITE_PLAYER)
				strcpy(myChessPieceTag, WHITE_PAWN_TAG);
			else
				strcpy(myChessPieceTag, BLACK_PAWN_TAG);

			break;

		default:

			strcpy(myChessPieceTag, EMPTY_SQUARE_TAG);
	}
}

MinichessErrorCode printChessboard(Minichess *myMinichess)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	if (myMinichess==NULL)
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		printf("\n");

		for (int row = MINICHESS_NUMBER_OF_ROWS - 1; row >= 0; row--)
	    {
		    printf("\n ");

	    	for (int i=0; i < MINICHESS_NUMBER_OF_COLUMNS * 6 + 1; i++)
	    		printf("-");

		    printf("\n");

			for (int column=0; column < MINICHESS_NUMBER_OF_COLUMNS; column++)
			{
				ChessPiece myChessPiece;

				if (returnValue==MINICHESS_RETURN_VALUE_OK)
					returnValue = getChessboardSquare(myMinichess, column, row, &myChessPiece);

				if (returnValue==MINICHESS_RETURN_VALUE_OK)
				{
					char myChessPieceTag[CHESS_PIECE_TAG_SIZE + 1];
					getChessPieceTag(myChessPiece.pieceName, myChessPiece.player, myChessPieceTag);

					printf(" | %s", myChessPieceTag);
				}
			}

			printf(" |");
		}

		printf("\n ");

		for (int i=0; i < MINICHESS_NUMBER_OF_COLUMNS * 6 + 1; i++)
			printf("-");

		printf("\n ");

		returnValue = printGameData(myMinichess);
	}

	return returnValue;
}

MinichessErrorCode printTrainingStatus(GSList *myNeuralNetworkList, int consecutiveVictories, int maxConsecutiveVictories)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	if (myNeuralNetworkList==NULL)
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		int listSize = g_slist_length(myNeuralNetworkList);

		printf("\n\n Consecutive victories: %d / %d\n", consecutiveVictories, maxConsecutiveVictories);
		printf("\n List size: %d\n\n", listSize);
	}

	return returnValue;
}

MinichessErrorCode printGameData(Minichess *myMinichess)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	int turnNumber;
	MinichessGameState myGameState;
	char myGameStateString[100] = {0};

	returnValue = getTurnNumber(myMinichess, &turnNumber);

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		returnValue = getGameState(myMinichess, &myGameState);

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		printf("\n\n Turn: %d\n", turnNumber);

		switch (myGameState)
		{
			case MINICHESS_GAME_STATE_WHITE_PLAYER_PLY:
				strcpy(myGameStateString, WHITE_PLAYER_PLY_TAG);
				break;

			case MINICHESS_GAME_STATE_BLACK_PLAYER_PLY:
				strcpy(myGameStateString, BLACK_PLAYER_PLY_TAG);
				break;

			case MINICHESS_GAME_STATE_WHITE_PLAYER_WINS:
				strcpy(myGameStateString, WHITE_PLAYER_WINS_TAG);
				break;

			case MINICHESS_GAME_STATE_BLACK_PLAYER_WINS:
				strcpy(myGameStateString, BLACK_PLAYER_WINS_TAG);
				break;

			case MINICHESS_GAME_STATE_STALEMATE:
				strcpy(myGameStateString, STALEMATE_TAG);
				break;
		}

		printf ("\n Game state: %s\n", myGameStateString);
	}

	return returnValue;
}

void printNewGameMessage()
{
	printf("\n\n    ------- NEW GAME -------\n");
}

void printTrainingCompletedMessage()
{
	printf("\n\n ----- Training completed -----\n\n");
}

void printHowToPlayMessage()
{
	printf("\n Hint: enter your movement as source column, source row, target column and target row separated by a blank space");
	printf("\n Column values: 0 - %d, row values: 0 - %d\n", MINICHESS_NUMBER_OF_COLUMNS - 1, MINICHESS_NUMBER_OF_ROWS - 1);
}

void printEnterYourMovementMessage()
{
	printf("\n\nEnter your movement:\n\n");
}

void printTRexSurrenderMessage()
{
	printf("\n[T-REX] I don't know how to move in this situation\n");
}

void printErrorMessage(MinichessErrorCode errorCode)
{
	printf("\n\n[ERROR] Error code: %d\n\n", errorCode);
}
