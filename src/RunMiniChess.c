/*
 ==================================================================================
 Name        : Minichess
 Author      : Kenshiro
 Version     : 1.00
 Copyright   : GNU General Public License (GPLv3)
 Description : T-Rex learns from scratch to play Minichess
 ==================================================================================
 */

#include "logic_tier/T-RexTrainer.h"
#include "presentation_tier/ConsoleManager.h"

#define T_REX_WAIT_SECONDS 1

static MinichessErrorCode playVsHuman(NeuralNetwork *myNeuralNetwork)
{
	Minichess *myMinichess;
	MinichessGameState myGameState = MINICHESS_GAME_STATE_WHITE_PLAYER_PLY;

	MinichessErrorCode returnValue = createMinichess(&myMinichess);

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		printTrainingCompletedMessage();
		printHowToPlayMessage();
		printNewGameMessage();

		returnValue = printChessboard(myMinichess);
	}

	while ((returnValue==MINICHESS_RETURN_VALUE_OK) &&
		   ((myGameState==MINICHESS_GAME_STATE_WHITE_PLAYER_PLY) ||
		    (myGameState==MINICHESS_GAME_STATE_BLACK_PLAYER_PLY)))
    {
		int sourceColumn, sourceRow, targetColumn, targetRow;

		ChessPiece myChessPiece;
		PlayerAction myPlayerAction;

		returnValue = MINICHESS_INVALID_MOVEMENT_ERROR;

		//Human player input (white player)
		while ((returnValue==MINICHESS_INVALID_MOVEMENT_ERROR) ||
			   (returnValue==MINICHESS_INVALID_PIECE_ERROR) ||
			   (returnValue==MINICHESS_HUMAN_PLAYER_INPUT_ERROR) ||
			   (returnValue==MINICHESS_INVALID_BOARD_DIMENSIONS_ERROR))
		{
			int auxChar = 0;

			returnValue = MINICHESS_RETURN_VALUE_OK;

			printEnterYourMovementMessage();

			int result = scanf("%d %d %d %d", &sourceColumn, &sourceRow, &targetColumn, &targetRow);

			//Clean the input buffer
			while ((auxChar!=EOF) && (auxChar!='\n'))
				auxChar = getchar();

			if (result==EOF)
				returnValue = MINICHESS_HUMAN_PLAYER_INPUT_ERROR;

			if (returnValue==MINICHESS_RETURN_VALUE_OK)
				returnValue = getChessboardSquare(myMinichess, sourceColumn, sourceRow, &myChessPiece);

			if (returnValue==MINICHESS_RETURN_VALUE_OK)
			{
				myPlayerAction.player = MINICHESS_WHITE_PLAYER;
				myPlayerAction.chessPieceName = myChessPiece.pieceName;
				myPlayerAction.targetColumn = targetColumn;
				myPlayerAction.targetRow = targetRow;

				returnValue = executePlayerAction(myMinichess, myPlayerAction);
			}

			if (returnValue!=MINICHESS_RETURN_VALUE_OK)
				printErrorMessage(returnValue);
		}

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = printChessboard(myMinichess);

		sleep(T_REX_WAIT_SECONDS);

		//T-Rex player input (black player)

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = invertChessboard(myMinichess);

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = computePlayerAction(myNeuralNetwork, myMinichess, MINICHESS_BLACK_PLAYER, &myPlayerAction);

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = executePlayerAction(myMinichess, myPlayerAction);

		if ((returnValue==MINICHESS_INVALID_MOVEMENT_ERROR) || (returnValue==MINICHESS_INVALID_PIECE_ERROR))
			printTRexSurrenderMessage();

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = invertChessboard(myMinichess);

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = printChessboard(myMinichess);

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = getGameState(myMinichess, &myGameState);
    }

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		returnValue = destroyMinichess(&myMinichess);

	return returnValue;
}

int main()
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;
	NeuralNetwork *myNeuralNetwork;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		returnValue = trainNeuralNetwork(&myNeuralNetwork);

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		returnValue = playVsHuman(myNeuralNetwork);

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		NeuralNetworkErrorCode result = destroyNeuralNetwork(&myNeuralNetwork);

		if (result!=NEURAL_NETWORK_RETURN_VALUE_OK)
			returnValue = MINICHESS_NEURAL_NETWORK_ERROR;
	}

	if (returnValue!=MINICHESS_RETURN_VALUE_OK)
		printErrorMessage(returnValue);

	return returnValue;
}
