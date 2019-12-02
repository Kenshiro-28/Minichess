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

static MinichessErrorCode playVsHuman(NeuralNetwork *myNeuralNetwork)
{
	Minichess *myMinichess;
	MinichessGameState myGameState = MINICHESS_GAME_STATE_WHITE_PLAYER_PLY;

	MinichessErrorCode returnValue = createMinichess(&myMinichess);

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		printf("\n\n ----- Training completed -----\n\n");
		printf("\n Hint: enter your movement as source column, source row, target column and target row separated by a blank space");
		printf("\n Column values: 0 - %d, row values: 0 - %d\n", MINICHESS_NUMBER_OF_COLUMNS - 1, MINICHESS_NUMBER_OF_ROWS - 1);

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

			printf("\n\nEnter your movement:\n\n");

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
			else
				printErrorMessage(returnValue);
		}

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = printChessboard(myMinichess);

		//T-Rex player input (black player)

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = invertChessboard(myMinichess);

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = computePlayerAction(myNeuralNetwork, myMinichess, MINICHESS_BLACK_PLAYER, &myPlayerAction);

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = executePlayerAction(myMinichess, myPlayerAction);

		if ((returnValue==MINICHESS_INVALID_MOVEMENT_ERROR) ||
			(returnValue==MINICHESS_INVALID_PIECE_ERROR))
			printf("\n[T-REX] I don't know how to move in this situation, I need more training\n");

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
