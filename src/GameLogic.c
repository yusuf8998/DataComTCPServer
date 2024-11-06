#include "GameLogic.h"

// GameLogic.h extern variables
cell_t board[9];
const cell_t EMPTY_CELL = 999;
const cell_t X_CELL = 0;
const cell_t O_CELL = 1;
const cell_t BOARD_FULL = 9999;
cell_t currentPlayer = X_CELL;

const cell_t FULL_SET_X = X_CELL * 3;
const cell_t FULL_SET_O = O_CELL * 3;

// 6 7 8
// 3 4 5
// 0 1 2

void clearBoard()
{
    for (int i = 0; i < 9; i++)
        board[i] = EMPTY_CELL;
    currentPlayer = X_CELL;
}

cell_t checkCellSum(cell_t sum)
{
    if (sum == FULL_SET_O)
        return O_CELL;
    else if (sum == FULL_SET_X)
        return X_CELL;
    return EMPTY_CELL;
}

cell_t checkBoardRow(int row)
{
    assert(row < 3);
    row *= 3;
    return checkCellSum( board[row] + board[row + 1] + board[row + 2] );
}

cell_t checkBoardColumn(int column)
{
    assert(column < 3);
    return checkCellSum( board[column] + board[column + 3] + board[column + 6] );
}

cell_t checkBoardDiagonal()
{
    return checkCellSum( board[0] + board[4] + board[8] );
}

cell_t checkBoardReverseDiagonal()
{
    return checkCellSum( board[2] + board[4] + board[6] );
}

cell_t checkBoard()
{
    cell_t result;
    for (int i = 0; i < 3; i++)
    {
        if ((result = checkBoardRow(i)) != EMPTY_CELL)
            return result;
        if ((result = checkBoardColumn(i)) != EMPTY_CELL)
            return result;
    }
    if ((result = checkBoardDiagonal()) != EMPTY_CELL)
        return result;
    if ((result = checkBoardReverseDiagonal()) != EMPTY_CELL)
        return result;
    result = 0;
    for (int i = 0; i < 9; i++)
        result += board[i];
    if (result < EMPTY_CELL)
        return BOARD_FULL;
    return EMPTY_CELL;
}

int getLineerBoardPosition(int x, int y)
{
    assert(x < 3 && y < 3);
    return (y * 3) + x;
}

int makeMove(int x, int y)
{
    int i = getLineerBoardPosition(x, y);
    if (board[i] != EMPTY_CELL)
        return -1;
    board[i] = currentPlayer;
    currentPlayer = (currentPlayer+1)%2;
    return 0;
}

void cellToString(char str[5], int x, int y)
{
    cell_t cell = board[getLineerBoardPosition(x, y)];
    if (cell == EMPTY_CELL)
        sprintf(str, "(%d,%d)", x, y);
    else
        sprintf(str, "%s", cell == O_CELL ? "  O  " : "  X  ");   
}

void printBoard()
{
    cell_t cell;
    for (int y = 2; y >= 0; y--)
    {
        for (int x = 0; x < 3; x++)
        {
            cell = board[getLineerBoardPosition(x, y)];
            if (cell == EMPTY_CELL)
                printf("(%d,%d) ", x, y);
            else
                printf("%s ", cell == O_CELL ? "  O  " : "  X  ");
        }
        printf("\n");
    }
    printf("-----------------\n");
}
