/*
 * ConsoleManager.h
 *
 *  Created on: Apr 25, 2018
 *      Author: kenshiro
 */

#ifndef SRC_PRESENTATION_TIER_CONSOLEMANAGER_H_
#define SRC_PRESENTATION_TIER_CONSOLEMANAGER_H_

#include "../logic_tier/Minichess.h"
#include <stdio.h>
#include <string.h>
#include <glib.h>

MinichessErrorCode printChessboard(Minichess *myMinichess);
MinichessErrorCode printTrainingStatus(GSList *myNeuralNetworkList, int consecutiveVictories, int maxConsecutiveVictories);
MinichessErrorCode printGameData(Minichess *myMinichess);
void printNewGameMessage();
void printTrainingCompletedMessage();
void printHowToPlayMessage();
void printEnterYourMovementMessage();
void printTRexSurrenderMessage();
void printErrorMessage(MinichessErrorCode errorCode);

#endif /* SRC_PRESENTATION_TIER_CONSOLEMANAGER_H_ */
