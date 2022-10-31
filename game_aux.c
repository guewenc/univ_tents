#include "header/game_aux.h"

#include <stdio.h>
#include <stdlib.h>

#include "header/game.h"
#include "header/game_ext.h"
#include "header/private.h"

// Prints a given square (row, col) of a given game
static void print_square(uint row, uint col, cgame g) {
    printf("%c", square_to_char(game_get_square(g, row, col)));
}

// Prints a given row of the game
static void print_row(uint row, cgame g) {
    printf("%d |", row);
    for (uint col = 0; col < game_nb_cols(g); col++)
        print_square(row, col, g);
    printf("| %d \n", game_get_expected_nb_tents_row(g, row));
}

// Pritns 'n' times the input 'c' char(s)
static void print_chars(uint n, char c) {
    for (uint i = 0; i < n; i++)
        printf("%c", c);
}

// Prints a line of 'n' dashes, surounded by 'spaces' spaces
static void dash_line(uint n, uint spaces) {
    print_chars(spaces, ' ');
    print_chars(n, '-');
    printf("\n");
}

// Prints numbers in ascending order from 'min' to 'max'
static void print_ascending(uint min, uint max) {
    for (unsigned int i = min; i < max; i++)
        printf("%d", i);
}

// Top end of the printed game
static void print_header(cgame g) {
    print_chars(3, ' ');
    print_ascending(0, game_nb_cols(g));
    printf("\n");
    dash_line(game_nb_cols(g), 3);
}

// Bottom end of the printed game
static void print_footer(cgame g) {
    dash_line(game_nb_cols(g), 3);
    print_chars(3, ' ');
    for (uint i = 0; i < game_nb_cols(g); i++)
        printf("%u", game_get_expected_nb_tents_col(g, i));
    printf("\n");
}

// Prints the whole game
void game_print(cgame g) {
    print_header(g);
    for (uint row = 0; row < game_nb_rows(g); row++)
        print_row(row, g);
    print_footer(g);
}

game game_default(void) {
    square default_squares[] = {
        EMPTY, EMPTY, EMPTY, EMPTY, TREE, TREE, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};

    uint nb_tent_row[] = {3, 0, 4, 0, 4, 0, 1, 0};
    uint nb_tent_col[] = {4, 0, 1, 2, 1, 1, 2, 1};

    return game_new(default_squares, nb_tent_row, nb_tent_col);
}

game game_default_solution(void) {
    square default_squares_solution[] = {
        TENT, GRASS, GRASS, TENT, TREE, TREE, TENT, GRASS, TREE, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, TREE, TENT, GRASS, GRASS, TENT, TREE, TENT, GRASS, TENT, TREE, GRASS, GRASS, GRASS, GRASS, TREE, GRASS, GRASS, TENT, TREE, TENT, GRASS, TENT, GRASS, TENT, GRASS, TREE, GRASS, GRASS, GRASS, TREE, GRASS, TREE, GRASS, TENT, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, TREE, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS};

    uint nb_tent_row[] = {3, 0, 4, 0, 4, 0, 1, 0};
    uint nb_tent_col[] = {4, 0, 1, 2, 1, 1, 2, 1};

    return game_new(default_squares_solution, nb_tent_row, nb_tent_col);
}