#pragma once

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef unsigned short cell_t;

extern cell_t board[9];
extern const cell_t EMPTY_CELL;
extern const cell_t X_CELL;
extern const cell_t O_CELL;
extern const cell_t BOARD_FULL;
extern cell_t currentPlayer;

extern void clearBoard();

extern cell_t checkCellSum(cell_t sum);

extern cell_t checkBoardRow(int row);
extern cell_t checkBoardColumn(int column);
extern cell_t checkBoardDiagonal();
extern cell_t checkBoardReverseDiagonal();

extern cell_t checkBoard();

extern int getLineerBoardPosition(int x, int y);
extern int makeMove(int x, int y);

extern void cellToString(char str[5], int x, int y);

extern void printBoard();

#endif // GAME_LOGIC_H