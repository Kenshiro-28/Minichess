/*
 * Minichess.h
 *
 *  Created on: Apr 25, 2018
 *      Author: kenshiro
 */

#ifndef SRC_LOGIC_TIER_MINICHESS_H_
#define SRC_LOGIC_TIER_MINICHESS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

#define MINICHESS_NUMBER_OF_ROWS 5
#define MINICHESS_NUMBER_OF_COLUMNS 5

#define MINICHESS_NUMBER_OF_SQUARES (MINICHESS_NUMBER_OF_COLUMNS * MINICHESS_NUMBER_OF_ROWS)

#define MINICHESS_NUMBER_OF_PIECES_OF_ALL_PLAYERS (MINICHESS_NUMBER_OF_PIECES * MINICHESS_NUMBER_OF_PLAYERS)

#define MINICHESS_MAXIMUM_NUMBER_OF_TURNS 40

typedef enum
{
	MINICHESS_WHITE_PLAYER,
	MINICHESS_BLACK_PLAYER,
	MINICHESS_NUMBER_OF_PLAYERS,
	MINICHESS_UNKNOWN_PLAYER
} MinichessPlayer;

typedef enum
{
    MINICHESS_TOWER,
    MINICHESS_KNIGHT,
    MINICHESS_BISHOP,
    MINICHESS_QUEEN,
    MINICHESS_KING,
    MINICHESS_PAWN_1,
    MINICHESS_PAWN_2,
    MINICHESS_PAWN_3,
    MINICHESS_PAWN_4,
    MINICHESS_PAWN_5,
    MINICHESS_NUMBER_OF_PIECES,
    MINICHESS_EMPTY_SQUARE
} MinichessPieceName;

typedef enum
{
	MINICHESS_GAME_STATE_WHITE_PLAYER_PLY,
	MINICHESS_GAME_STATE_BLACK_PLAYER_PLY,
	MINICHESS_GAME_STATE_WHITE_PLAYER_WINS,
	MINICHESS_GAME_STATE_BLACK_PLAYER_WINS,
	MINICHESS_GAME_STATE_STALEMATE
} MinichessGameState;

typedef enum
{
	MINICHESS_RETURN_VALUE_OK = 0,
	MINICHESS_NULL_POINTER_ERROR = -1,
	MINICHESS_MEMORY_ALLOCATION_ERROR = -2,
	MINICHESS_INVALID_PLAYER_ERROR = -3,
	MINICHESS_INVALID_PIECE_ERROR = -4,
	MINICHESS_NOT_PLAYER_TURN_ERROR = -5,
	MINICHESS_INVALID_MOVEMENT_ERROR = -6,
	MINICHESS_INVALID_BOARD_DIMENSIONS_ERROR = -7,
	MINICHESS_GAME_OVER_ERROR = -8,
	MINICHESS_NEURAL_NETWORK_ERROR = -9,
	MINICHESS_HUMAN_PLAYER_INPUT_ERROR = -10
} MinichessErrorCode;

typedef struct miniChess Minichess;

typedef struct chessPiece
{
	MinichessPlayer player;
	MinichessPieceName pieceName;
	bool isAlive;
	int column;
	int row;
} ChessPiece;

typedef struct playerAction
{
	MinichessPlayer player;
	MinichessPieceName chessPieceName;
	int targetColumn;
	int targetRow;
} PlayerAction;

MinichessErrorCode createMinichess(Minichess **myMinichess);
MinichessErrorCode destroyMinichess(Minichess **myMinichess);
MinichessErrorCode resetMinichess(Minichess *myMinichess);
MinichessErrorCode copyMinichess(Minichess *myMinichess, Minichess *myMinichessCopy);
MinichessErrorCode executePlayerAction(Minichess *myMinichess, PlayerAction myPlayerAction);
MinichessErrorCode getTurnNumber(Minichess *myMinichess, int *turnNumber);
MinichessErrorCode getChessboardSquare(Minichess *myMinichess, int column, int row, ChessPiece *myChessPiece);
MinichessErrorCode getChessPiece(Minichess *myMinichess, MinichessPlayer myPlayer, MinichessPieceName myChessPieceName, ChessPiece *myChessPiece);
MinichessErrorCode getGameState(Minichess *myMinichess, MinichessGameState *myGameState);
MinichessErrorCode invertChessboard(Minichess *myMinichess);

#endif /* SRC_LOGIC_TIER_MINICHESS_H_ */
