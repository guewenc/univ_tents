/**
 * @file private_func.h
 * @brief Utilitary functions used in game.c, game_tools.c and game_aux.c files.
 **/

#ifndef UTILITARY_H
#define UTILITARY_H

#include "game.h"

/**
 * @brief Value of an unvalid square & base stack size.
 **/
#define INIT_STACK_SIZE 2

/**
 * @brief Structure used to store row/col coordinates of a square.
 **/
typedef struct coor {
    uint row, col;
} coor;

/**
 * @brief Different kinds of directions from a square.
 **/
typedef enum {
    NONE, NORTH, SOUTH, WEST, EAST, NORTH_WEST, NORTH_EAST, SOUTH_WEST, SOUTH_EAST
} direction;


/**
 * @brief Stores the coordinates of a square, its value before it was modified (old) and its current value (new).
 **/
typedef struct move {
    uint x, y;
    square old, new;
} move;

/**
 * @brief Stack structure.
 **/
typedef struct stack {
    struct move *content;             // Array of moves
    unsigned int size;                // Maximum number of elements in the stack
    unsigned int next_available_pos;  // Next available position for stacking
} * stack;

/**
 * @brief Settings of the game.
 **/
typedef struct param {
    uint nb_rows, nb_cols;
    bool wrapping, diagadj;
} param;

/**
 * @brief History management using a stack.
 **/
typedef struct d_stack {
    stack redo, undo;
} d_stack;

/**
 * @brief Game structure.
 **/
typedef struct game_s {
    square *squares;
    uint *nb_tents_row;
    uint *nb_tents_col;

    param *settings;
    d_stack *action;
} game_s;

/* ************************************************************************** */
/*                              GENERAL FUNCTIONS                             */
/* ************************************************************************** */

/**
 * @brief Prints an error and quits the program.
 **/
void display_error_and_exit(char argv[]);

/**
 * @brief Converting a character (char) to a square
 **/
square char_to_square(int square);

/**
 * @brief Converting a square to a character (char)
 **/
char square_to_char(square current_square);

/**
 * @brief Verifies the input parameters.
 **/
void valid_input(square *squares, uint *nb_tents_row, uint *nb_tents_col, uint nb_rows, uint nb_cols);

/**
 * @brief Checks if (i,j) square is an s square type.
 **/
bool check_square_value(cgame g, uint i, uint j, square s);

/**
 * @brief Checks if (i,j) square is an EMPTY square.
 **/
bool check_square_empty(cgame g, uint i, uint j);

/**
 * @brief Checks if (i,j) square is a GRASS square.
 **/
bool check_square_grass(cgame g, uint i, uint j);

/**
 * @brief Checks if (i,j) square is a TENT square.
 **/
bool check_square_tent(cgame g, uint i, uint j);

/**
 * @brief Checks if (i,j) square is a TREE square.
 **/
bool check_square_tree(cgame g, uint i, uint j);

/**
 * @brief Fills a given row with s squares.
 **/
void fill_square_row(game g, uint i, square s);

/**
 * @brief Fills a given column with s squares.
 **/
void fill_square_col(game g, uint j, square s);

/**
 * @brief Counts the number of s squares in a given row.
 **/
uint nb_square_row(cgame g, uint i, square s);

/**
 * @brief Counts the number of s squares in a given column.
 **/
uint nb_square_col(cgame g, uint j, square s);

/**
 * @brief Counts the number of s squares in a given game.
 **/
uint nb_square_all(cgame g, square s);

/* ************************************************************************** */
/*                     DIRECTION / COORDINATES FUNCTIONS                      */
/* ************************************************************************** */

/**
 * @brief Returns the array index of the square array from a game of a given (i,j) position.
 * @detail For instance in the [EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY, EMPTY, EMPTY] table,
 *         representing a game with 2 rows and 4 col, get_array_index(g, 0, 3) -> EMPTY and
 *         get_array_index(g, 1, 0) -> TREE.
 **/
uint get_array_index(cgame g, uint i, uint j);

/**
 * @brief Get_array_index function without the need for a game.
 **/
uint get_array_index_aux(uint i, uint j, uint nb_cols);

/**
 * @brief Checks that (i,j) position is a valid position in the g game board.
 **/
bool check_coor(cgame g, int i, int j);

/**
 * @brief Creates a coordinate with row and column numbers.
 **/
coor make_coor(int row, int col);

/**
 * @brief Retrieves the row number of a given coordinate.
 **/
int row_of_coor(coor cor);

/**
 * @brief Retrieves the column number of a given coordinate.
 **/
int col_of_coor(coor cor);

/**
 * @brief Returns a coordinate based on (i,j) position and dir direction shift.
 **/
coor coor_to_dir(int i, int j, direction dir);

/**
 * @brief Transforms a given direction into a coordinate shift.
 **/
coor direction_to_coor(direction dir);

/**
 * @brief Put a square in the dir direction of (i, j).
 **/
bool neigh_set_square(game g, uint i, uint j, direction dir, square s);

/**
 * @brief Returns true if the square in the dir direction of (i, j) is s.
 **/
bool neigh_check_square(cgame g, uint i, uint j, direction dir, square s);

/**
 * @brief Checks that at least one adjacent square (orthogonally ONLY) to (i,j) square is of type square (s).
 **/
bool neigh_get_square(cgame g, uint i, uint j, square s, bool diagadj);

/**
 * @brief Count the number of squares s around (i, j).
 **/
uint neigh_count(cgame g, uint i, uint j, square s, bool diag);

/**
 * @brief Count the number of valid squares (understand here the number of squares not bordered by the game) around (i, j).
 **/
uint neigh_count_valid(cgame g, int i, int j);

/**
 * @brief Checks that all adjacent squares (orthogonally ONLY) to (i,j) square are of type square (s).
 **/
bool neigh_all_get_square(cgame g, uint i, uint j, square s);

/* ************************************************************************** */
/*                    CHECK RULES / GAME IS OVER FUNCTIONS                    */
/* ************************************************************************** */

/**
 * @brief Checks that expected and currently placed tents in a row matches.
 **/
bool check_expected_nb_tents_row(cgame g, uint i);

/**
 * @brief Checks that expected and currently placed tents in a column matches.
 **/
bool check_expected_nb_tents_col(cgame g, uint j);

/**
 * @brief Checks that expected and currently placed tents in a whole game matches.
 **/
bool check_expected_nb_tents_all(cgame g);

/**
 * @brief Checks rule 1 : no tents are orthogonally adjacent (+ diagonally if diagadj is enabled).
 **/
bool check_rule1(cgame g);

/**
 * @brief Checks rule 2 : The number of tents in each row, and in each column, matches the expected numbers given around the sides of the grid.
 **/
bool check_rule2(cgame g);

/**
 * @brief Checks rule 3 : There are exactly as many tents as trees.
 **/
bool check_rule3(cgame g);

/**
 * @brief Checks rule 4 : Each tent must be orthogonally adjacent to at least one tree and vice versa.
 **/
bool check_rule4(cgame g);

/* ************************************************************************** */
/*                              STACK FUNCTIONS                               */
/* ************************************************************************** */

/**
 * @brief Creates an empty stack.
 **/
stack stack_create_empty();

/**
 * @brief Tests whether a stack is empty
 **/
bool stack_is_empty(stack s);

/**
 * @brief Frees the stack.
 **/
void stack_free(stack s);

/**
 * @brief Returns the value on top of the stack.
 **/
move stack_top_value(stack s);

/**
 * @brief Adds an element on top of the stack.
 **/
stack stack_push(stack s, move data);

/**
 * @brief Erases the element on top of the stack.
 **/
stack stack_pop(stack s);

/**
 * @brief Removes all elements of the stack.
 **/
void stack_dump(stack s);

/**
 * @brief Creates a move variable, used by the stack for undo and redo functions.
 **/
move make_move(cgame g, uint i, uint j, square s);

/* ************************************************************************** */
/*                             CREATE RANDOM GAME                             */
/* ************************************************************************** */

/**
 * @brief Generates a new random game based on the data passed to it as parameters.
 * @param nb_rows Line size
 * @param nb_cols Column size
 * @param tree_to_place Number of trees to place
 * @param wrapping True if game is wrapping - false otherwise
 * @param diagadj True if game is diagadj - false otherwise
 **/
game generate_random(uint nb_rows, uint nb_cols, uint tree_to_place, bool wrapping, bool diagadj);

#endif