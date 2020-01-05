/*
 * T-RexTrainer.c
 *
 *  Created on: May 21, 2018
 *      Author: kenshiro
 */

#include "T-RexTrainer.h"

static MinichessErrorCode computeChessPieceInput(NeuralNetwork *myNeuralNetwork, int neuralNetworkInputIndex, bool isAlive, int numberOfSquare)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	NeuronData myChessPieceStatus[CHESS_PIECE_STATUS_BITS];

	if (isAlive)
		numberOfSquare++;
	else
		numberOfSquare = 0;

	for (int i = CHESS_PIECE_STATUS_BITS - 1; i >= 0; i--)
	{
		int bit = numberOfSquare >> i;

		if (bit & 1)
			myChessPieceStatus[i] = NEURON_DATA_ONE;
		else
			myChessPieceStatus[i] = NEURON_DATA_ZERO;
	}

	int bitNumber = CHESS_PIECE_STATUS_BITS - 1;

	while ((bitNumber>=0) && (returnValue==MINICHESS_RETURN_VALUE_OK))
	{
		NeuralNetworkErrorCode result = setNeuralNetworkInput(myNeuralNetwork, neuralNetworkInputIndex, myChessPieceStatus[bitNumber]);

		if (result!=NEURAL_NETWORK_RETURN_VALUE_OK)
			returnValue = MINICHESS_NEURAL_NETWORK_ERROR;

		bitNumber--;
		neuralNetworkInputIndex++;
	}

	return returnValue;
}

static MinichessErrorCode computeTRexInput(Minichess *myMinichess, MinichessPlayer myPlayer, NeuralNetwork *myNeuralNetwork)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	int neuralNetworkInputIndex = 0;

	for (MinichessPlayer playerIndex=0; playerIndex<MINICHESS_NUMBER_OF_PLAYERS; playerIndex++)
		for (MinichessPieceName chessPieceName=0; chessPieceName<MINICHESS_NUMBER_OF_PIECES; chessPieceName++)
		{
			ChessPiece myChessPiece;

			if (returnValue==MINICHESS_RETURN_VALUE_OK)
				returnValue = getChessPiece(myMinichess, playerIndex, chessPieceName, &myChessPiece);

			if (returnValue==MINICHESS_RETURN_VALUE_OK)
			{
				int fixedIndex = neuralNetworkInputIndex;
				int numberOfSquare = myChessPiece.column + myChessPiece.row * MINICHESS_NUMBER_OF_COLUMNS;

				//If the neural network plays with black pieces, you must first enter the status of the black pieces
				//and then the status of the white pieces.
				if (myPlayer==MINICHESS_BLACK_PLAYER)
				{
					if (playerIndex==MINICHESS_WHITE_PLAYER)
						fixedIndex = neuralNetworkInputIndex + (CHESS_PIECE_STATUS_BITS * MINICHESS_NUMBER_OF_PIECES);
					else if (playerIndex==MINICHESS_BLACK_PLAYER)
						fixedIndex = neuralNetworkInputIndex - (CHESS_PIECE_STATUS_BITS * MINICHESS_NUMBER_OF_PIECES);
				}

				returnValue = computeChessPieceInput(myNeuralNetwork, fixedIndex, myChessPiece.isAlive, numberOfSquare);
			}

			if ((neuralNetworkInputIndex<NEURAL_NETWORK_NUMBER_OF_INPUTS) && (returnValue==MINICHESS_RETURN_VALUE_OK))
				neuralNetworkInputIndex = neuralNetworkInputIndex + CHESS_PIECE_STATUS_BITS;
		}

	if (neuralNetworkInputIndex!=NEURAL_NETWORK_NUMBER_OF_INPUTS)
		returnValue = MINICHESS_NEURAL_NETWORK_ERROR;

	return returnValue;
}

static MinichessErrorCode parseTRexOutput(NeuronData *neuralNetworkOutput, int numberOfOutputs, PlayerAction *myPlayerAction)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	MinichessPieceName myChessPieceName = 0;

	int targetSquare = 0;

	int neuralNetworkOutputIndex = 0;

	while (neuralNetworkOutputIndex < CHESS_PIECE_NAME_BITS)
	{
		int power = CHESS_PIECE_NAME_BITS - 1 - neuralNetworkOutputIndex;

		if (neuralNetworkOutput[neuralNetworkOutputIndex]==NEURON_DATA_ONE)
			myChessPieceName = myChessPieceName + pow(2, power);

		neuralNetworkOutputIndex++;
	}

	while (neuralNetworkOutputIndex < numberOfOutputs)
	{
		int power = CHESS_PIECE_STATUS_BITS - 2 - (neuralNetworkOutputIndex - CHESS_PIECE_NAME_BITS);

		if (neuralNetworkOutput[neuralNetworkOutputIndex]==NEURON_DATA_ONE)
			targetSquare = targetSquare + pow(2, power);

		neuralNetworkOutputIndex++;
	}

	if ((myChessPieceName>=MINICHESS_NUMBER_OF_PIECES) || (targetSquare>=MINICHESS_NUMBER_OF_SQUARES))
		returnValue = MINICHESS_INVALID_MOVEMENT_ERROR;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		myPlayerAction->player = MINICHESS_UNKNOWN_PLAYER;
		myPlayerAction->chessPieceName = myChessPieceName;
		myPlayerAction->targetColumn = targetSquare % MINICHESS_NUMBER_OF_COLUMNS;
		myPlayerAction->targetRow = targetSquare / MINICHESS_NUMBER_OF_COLUMNS;
	}

	return returnValue;
}

static MinichessErrorCode playMinichessGame(Minichess *myMinichess, NeuralNetwork *whitePlayerNeuralNetwork, NeuralNetwork *blackPlayerNeuralNetwork, MinichessGameState *myGameState)
{
	MinichessErrorCode returnValue = resetMinichess(myMinichess);

	printNewGameMessage();

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		returnValue = getGameState(myMinichess, myGameState);

	while ((returnValue==MINICHESS_RETURN_VALUE_OK) &&
		   ((*myGameState==MINICHESS_GAME_STATE_WHITE_PLAYER_PLY) ||
		    (*myGameState==MINICHESS_GAME_STATE_BLACK_PLAYER_PLY)))
    {
		NeuralNetwork *myNeuralNetwork;
		MinichessPlayer myPlayer;
		PlayerAction myPlayerAction = {INT_MIN, INT_MIN, INT_MIN, INT_MIN};

		if (*myGameState==MINICHESS_GAME_STATE_WHITE_PLAYER_PLY)
		{
			myPlayer = MINICHESS_WHITE_PLAYER;
			myNeuralNetwork = whitePlayerNeuralNetwork;
		}
		else if (*myGameState==MINICHESS_GAME_STATE_BLACK_PLAYER_PLY)
		{
			myPlayer = MINICHESS_BLACK_PLAYER;
			myNeuralNetwork = blackPlayerNeuralNetwork;

			returnValue = invertChessboard(myMinichess);
		}

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = computePlayerAction(myNeuralNetwork, myMinichess, myPlayer, &myPlayerAction);

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = executePlayerAction(myMinichess, myPlayerAction);

		//If the movement is invalid, the other player wins
		if (returnValue==MINICHESS_INVALID_MOVEMENT_ERROR)
		{
			returnValue = MINICHESS_RETURN_VALUE_OK;

			if (myPlayer==MINICHESS_WHITE_PLAYER)
				*myGameState = MINICHESS_GAME_STATE_BLACK_PLAYER_WINS;
			else if (myPlayer==MINICHESS_BLACK_PLAYER)
				*myGameState = MINICHESS_GAME_STATE_WHITE_PLAYER_WINS;
		}
		else if (returnValue==MINICHESS_RETURN_VALUE_OK)
		{
			returnValue = getGameState(myMinichess, myGameState);

			if (returnValue==MINICHESS_RETURN_VALUE_OK)
				returnValue = printChessboard(myMinichess);
		}

		if ((myPlayer==MINICHESS_BLACK_PLAYER) && (returnValue==MINICHESS_RETURN_VALUE_OK))
			returnValue = invertChessboard(myMinichess);
	}

	return returnValue;
}

static MinichessErrorCode playAgainstPreviousWinners(Minichess *myMinichess, NeuralNetwork *myNeuralNetwork, GSList *myNeuralNetworkList, bool *isNewWinner)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	MinichessGameState myGameState;

	bool myNeuralNetworkWins = true;

	int listSize = g_slist_length(myNeuralNetworkList);

	int i=0;

	while ((i<listSize) && (myNeuralNetworkWins) && (returnValue==MINICHESS_RETURN_VALUE_OK))
	{
		NeuralNetwork *enemyNeuralNetwork = g_slist_nth_data(myNeuralNetworkList, i);

		myNeuralNetworkWins = false;

		//Play as white player
		returnValue = playMinichessGame(myMinichess, myNeuralNetwork, enemyNeuralNetwork, &myGameState);

		//Play as black player
		if ((returnValue==MINICHESS_RETURN_VALUE_OK) && (myGameState==MINICHESS_GAME_STATE_WHITE_PLAYER_WINS))
		{
			returnValue = playMinichessGame(myMinichess, enemyNeuralNetwork, myNeuralNetwork, &myGameState);

			if ((returnValue==MINICHESS_RETURN_VALUE_OK) && (myGameState==MINICHESS_GAME_STATE_BLACK_PLAYER_WINS))
				myNeuralNetworkWins = true;
		}

		i++;
	}

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		if (myNeuralNetworkWins)
			*isNewWinner = true;
		else
			*isNewWinner = false;
	}

	return returnValue;
}

static MinichessErrorCode addNeuralNetworkToList(NeuralNetwork *myNeuralNetwork, GSList **myNeuralNetworkList)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	NeuralNetwork *newWinner;

	NeuralNetworkErrorCode result = createNeuralNetwork(&newWinner, NEURAL_NETWORK_NUMBER_OF_INPUTS, NEURAL_NETWORK_NUMBER_OF_HIDDEN_LAYERS, NEURAL_NETWORK_NUMBER_OF_OUTPUTS);

	if (result==NEURAL_NETWORK_RETURN_VALUE_OK)
		result = cloneNeuralNetwork(myNeuralNetwork, newWinner);

	if (result!=NEURAL_NETWORK_RETURN_VALUE_OK)
		returnValue = MINICHESS_NEURAL_NETWORK_ERROR;

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		*myNeuralNetworkList = g_slist_prepend(*myNeuralNetworkList, newWinner);

	return returnValue;
}

MinichessErrorCode trainNeuralNetwork(NeuralNetwork **myNeuralNetwork)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	Minichess *myMinichess;

	NeuralNetwork *bestNeuralNetwork = NULL;
	NeuralNetwork *offspringNeuralNetwork = NULL;

	NeuralNetworkErrorCode result;

	bool trainingCompleted = false;

	int consecutiveVictories = 0;

	//The list of neural networks that have defeated the previous ones
	GSList *myNeuralNetworkList = NULL;

	if (myNeuralNetwork==NULL)
		returnValue = MINICHESS_NULL_POINTER_ERROR;

	//Create Minichess
	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		returnValue = createMinichess(&myMinichess);

	//Check if a neural network of a previous training already exists
	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		struct stat fileStatus;

		if (!stat(NEURAL_NETWORK_FILE_NAME, &fileStatus))
			result = loadNeuralNetwork(NEURAL_NETWORK_FILE_NAME, &bestNeuralNetwork);
		else
			result = createNeuralNetwork(&bestNeuralNetwork, NEURAL_NETWORK_NUMBER_OF_INPUTS, NEURAL_NETWORK_NUMBER_OF_HIDDEN_LAYERS, NEURAL_NETWORK_NUMBER_OF_OUTPUTS);

		if (result!=NEURAL_NETWORK_RETURN_VALUE_OK)
			returnValue = MINICHESS_NEURAL_NETWORK_ERROR;
	}

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		returnValue = addNeuralNetworkToList(bestNeuralNetwork, &myNeuralNetworkList);

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		result = createNeuralNetwork(&offspringNeuralNetwork, NEURAL_NETWORK_NUMBER_OF_INPUTS, NEURAL_NETWORK_NUMBER_OF_HIDDEN_LAYERS, NEURAL_NETWORK_NUMBER_OF_OUTPUTS);

		if (result!=NEURAL_NETWORK_RETURN_VALUE_OK)
			returnValue = MINICHESS_NEURAL_NETWORK_ERROR;
	}

	while (!trainingCompleted && (returnValue==MINICHESS_RETURN_VALUE_OK))
	{
		bool offspringWins = false;

		result = cloneNeuralNetwork(bestNeuralNetwork, offspringNeuralNetwork);

		if (result==NEURAL_NETWORK_RETURN_VALUE_OK)
			result = mutateNeuralNetwork(offspringNeuralNetwork);

		if (result!=NEURAL_NETWORK_RETURN_VALUE_OK)
			returnValue = MINICHESS_NEURAL_NETWORK_ERROR;

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = playAgainstPreviousWinners(myMinichess, offspringNeuralNetwork, myNeuralNetworkList, &offspringWins);

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
		{
			if (offspringWins)
			{
				consecutiveVictories = 0;

				//Select offspring to continue the evolutionary process
				NeuralNetwork *auxNeuralNetwork = bestNeuralNetwork;
				bestNeuralNetwork = offspringNeuralNetwork;
				offspringNeuralNetwork = auxNeuralNetwork;

				returnValue = addNeuralNetworkToList(bestNeuralNetwork, &myNeuralNetworkList);
			}
			else
			{
				consecutiveVictories++;

				if (consecutiveVictories>=NUMBER_OF_CONSECUTIVE_VICTORIES)
				{
					trainingCompleted = true;
					*myNeuralNetwork = bestNeuralNetwork;
				}
			}
		}

		if (returnValue==MINICHESS_RETURN_VALUE_OK)
			returnValue = printTrainingStatus(myNeuralNetworkList, consecutiveVictories);
	}

	//Save the neural network when training ends
	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		result = saveNeuralNetwork(NEURAL_NETWORK_FILE_NAME, bestNeuralNetwork);

		if (result!=NEURAL_NETWORK_RETURN_VALUE_OK)
			returnValue = MINICHESS_NEURAL_NETWORK_ERROR;
	}

	//Destroy Minichess
	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		returnValue = destroyMinichess(&myMinichess);

	//Destroy offspring neural network
	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		result = destroyNeuralNetwork(&offspringNeuralNetwork);

		if (result!=NEURAL_NETWORK_RETURN_VALUE_OK)
			returnValue = MINICHESS_NEURAL_NETWORK_ERROR;
	}

	//Destroy neural network list
	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		int listSize = g_slist_length(myNeuralNetworkList);

		int i=0;

		while ((i<listSize) && (returnValue==MINICHESS_RETURN_VALUE_OK))
		{
			NeuralNetwork *auxNeuralNetwork = g_slist_nth_data(myNeuralNetworkList, i);

			result = destroyNeuralNetwork(&auxNeuralNetwork);

			if (result!=NEURAL_NETWORK_RETURN_VALUE_OK)
				returnValue = MINICHESS_NEURAL_NETWORK_ERROR;

			i++;
		}

		g_slist_free(myNeuralNetworkList);
	}

	return returnValue;
}

MinichessErrorCode computePlayerAction(NeuralNetwork *myNeuralNetwork, Minichess *myMinichess, MinichessPlayer myPlayer, PlayerAction *myPlayerAction)
{
	MinichessErrorCode returnValue = MINICHESS_RETURN_VALUE_OK;

	NeuronData *neuralNetworkOutput;

	int numberOfOutputs;

	returnValue = computeTRexInput(myMinichess, myPlayer, myNeuralNetwork);

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
	{
		NeuralNetworkErrorCode result = computeNeuralNetworkOutput(myNeuralNetwork, &neuralNetworkOutput, &numberOfOutputs);

		if ((result!=NEURAL_NETWORK_RETURN_VALUE_OK) || (numberOfOutputs!=NEURAL_NETWORK_NUMBER_OF_OUTPUTS))
			returnValue = MINICHESS_NEURAL_NETWORK_ERROR;
	}

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		returnValue = parseTRexOutput(neuralNetworkOutput, numberOfOutputs, myPlayerAction);

	if (returnValue==MINICHESS_RETURN_VALUE_OK)
		myPlayerAction->player = myPlayer;

	return returnValue;
}
