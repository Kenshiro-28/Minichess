/*
 * T-RexTrainer.h
 *
 *  Created on: May 21, 2018
 *      Author: kenshiro
 */

#ifndef SRC_LOGIC_TIER_T_REXTRAINER_H_
#define SRC_LOGIC_TIER_T_REXTRAINER_H_

#include "Minichess.h"
#include "../presentation_tier/ConsoleManager.h"

#include <T-Rex/logic_tier/NeuralNetwork.h>
#include <T-Rex/data_tier/DataManager.h>
#include <glib.h>
#include <sys/stat.h>
#include <math.h>

//The amount of consecutive games won to consider the training completed
#define NUMBER_OF_CONSECUTIVE_VICTORIES 10000000

//Number of bits to represent the chess piece alive status and its position expressed in binary numeral system
#define CHESS_PIECE_STATUS_BITS ((int) (1 + ceil(log2(MINICHESS_NUMBER_OF_SQUARES))))

//Number of bits to represent the chess piece name expressed in binary numeral system
#define CHESS_PIECE_NAME_BITS ((int) ceil(log2(MINICHESS_NUMBER_OF_PIECES)))

/*
 * The input layer has the status of the player's pieces and then the opponent's pieces.
 * For a 5 x 5 board there are 6 inputs per piece. Since there are 10 pieces per player
 * and there are 2 players, the total number of inputs is 120.
 */

#define NEURAL_NETWORK_NUMBER_OF_INPUTS (CHESS_PIECE_STATUS_BITS * MINICHESS_NUMBER_OF_PIECES * MINICHESS_NUMBER_OF_PLAYERS)

#define NEURAL_NETWORK_NUMBER_OF_HIDDEN_LAYERS 30

/*
 * The output of the neural network is the name of the piece that moves and its new position.
 * For a 5 x 5 board there are 4 outputs for the name of the piece and 5 for its new position.
 * The total number of outputs is 9.
*/

#define NEURAL_NETWORK_NUMBER_OF_OUTPUTS (CHESS_PIECE_NAME_BITS + CHESS_PIECE_STATUS_BITS - 1)

#define NEURAL_NETWORK_FILE_NAME "neural_network.json"

MinichessErrorCode computePlayerAction(NeuralNetwork *myNeuralNetwork, Minichess *myMinichess, MinichessPlayer myPlayer, PlayerAction *myPlayerAction);
MinichessErrorCode trainNeuralNetwork(NeuralNetwork **myNeuralNetwork);

#endif /* SRC_LOGIC_TIER_T_REXTRAINER_H_ */
