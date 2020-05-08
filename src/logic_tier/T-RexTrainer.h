/*
 * T-RexTrainer.h
 *
 *  Created on: May 21, 2018
 *      Author: kenshiro
 */

#ifndef SRC_LOGIC_TIER_T_REXTRAINER_H_
#define SRC_LOGIC_TIER_T_REXTRAINER_H_

#include "../presentation_tier/ConsoleManager.h"
#include <T-Rex/logic_tier/NeuralNetwork.h>
#include <T-Rex/data_tier/DataManager.h>
#include <glib.h>
#include <sys/stat.h>
#include <math.h>

//The amount of consecutive games won to consider the training completed
#define MAX_CONSECUTIVE_VICTORIES 100000000

/*
 *Number of bits to represent the chess piece position expressed in binary numeral system.
 *The position is increased by one, so the number zero means that the chess piece is dead.
 *For a 5 x 5 board there are 5 status bits per piece.
 */

#define CHESS_PIECE_STATUS_BITS ((int) (ceil(log2(MINICHESS_NUMBER_OF_SQUARES + 1))))

//Number of bits to represent the chess piece name expressed in binary numeral system
#define CHESS_PIECE_NAME_BITS ((int) ceil(log2(MINICHESS_NUMBER_OF_PIECES)))

/*
 * The input layer receives the status of the player's pieces and then the opponent's pieces.
 * For a 5 x 5 board there are 5 inputs per piece. Since there are 10 pieces per player
 * and there are 2 players, the total number of inputs is 100.
 */

#define NEURAL_NETWORK_NUMBER_OF_INPUTS (CHESS_PIECE_STATUS_BITS * MINICHESS_NUMBER_OF_PIECES * MINICHESS_NUMBER_OF_PLAYERS)

#define NEURAL_NETWORK_NUMBER_OF_HIDDEN_LAYERS 100

/*
 * The output layer provides the code of the piece that moves and its new position.
 * For a 5 x 5 board there are 4 outputs for the code of the piece and 5 for its new position.
 * The total number of outputs is 9.
*/

#define NEURAL_NETWORK_NUMBER_OF_OUTPUTS (CHESS_PIECE_NAME_BITS + CHESS_PIECE_STATUS_BITS)

#define NEURAL_NETWORK_FILE_NAME "neural_network.json"

MinichessErrorCode computePlayerAction(NeuralNetwork *myNeuralNetwork, Minichess *myMinichess, MinichessPlayer myPlayer, PlayerAction *myPlayerAction);
MinichessErrorCode trainNeuralNetwork(NeuralNetwork **myNeuralNetwork);

#endif /* SRC_LOGIC_TIER_T_REXTRAINER_H_ */
