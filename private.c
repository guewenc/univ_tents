#include "header/private.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "header/game.h"
#include "header/game_ext.h"

/* ************************************************************************** */
/*                              GENERAL FUNCTIONS                             */
/* ************************************************************************** */

void display_error_and_exit(char argv[]) {
    fprintf(stderr, "Fatal error: %s\n", argv);
    exit(EXIT_FAILURE);
}

square char_to_square(int square) {
    switch (square) {
        case ' ': return EMPTY;
        case 'x': return TREE;
        case '-': return GRASS;
        case '*': return TENT;
        default: return EMPTY;
    }
}

char square_to_char(square current_square) {
    switch (current_square) {
        case EMPTY: return ' ';
        case TREE: return 'x';
        case GRASS: return '-';
        case TENT: return '*';
        default: return ' ';
    }
}

void valid_input(square *squares, uint *nb_tents_row, uint *nb_tents_col, uint nb_rows, uint nb_cols) {
    if (!squares)
        display_error_and_exit("\"squares\" pointer shouldn't be null");
    if (!nb_tents_row)
        display_error_and_exit("\"nb_tents_row\" pointer shouldn't be null");
    if (!nb_tents_col)
        display_error_and_exit("\"nb_tents_col\" pointer shouldn't be null");
    if (nb_rows == 0)
        display_error_and_exit("\"nb_rows\" should be different than 0");
    if (nb_cols == 0)
        display_error_and_exit("\"nb_cols\" should be different than 0");
}

bool check_square_value(cgame g, uint i, uint j, square s) {
    return game_get_square(g, i, j) == s;
}
bool check_square_empty(cgame g, uint i, uint j) { return check_square_value(g, i, j, EMPTY); }
bool check_square_grass(cgame g, uint i, uint j) { return check_square_value(g, i, j, GRASS); }
bool check_square_tent(cgame g, uint i, uint j) { return check_square_value(g, i, j, TENT); }
bool check_square_tree(cgame g, uint i, uint j) { return check_square_value(g, i, j, TREE); }

void fill_square_row(game g, uint i, square s) {
    for (uint j = 0; j < game_nb_cols(g); j++)
        if (check_square_empty(g, i, j))
            game_play_move(g, i, j, s);
}

void fill_square_col(game g, uint j, square s) {
    for (uint i = 0; i < game_nb_rows(g); i++)
        if (check_square_empty(g, i, j))
            game_play_move(g, i, j, s);
}

uint nb_square_row(cgame g, uint i, square s) {
    uint cpt = 0;
    for (uint j = 0; j < game_nb_cols(g); j++)
        if (check_square_value(g, i, j, s))
            cpt++;
    return cpt;
}

uint nb_square_col(cgame g, uint j, square s) {
    uint cpt = 0;
    for (uint i = 0; i < game_nb_rows(g); i++)
        if (check_square_value(g, i, j, s))
            cpt++;
    return cpt;
}

uint nb_square_all(cgame g, square s) {
    uint cpt = 0;
    for (uint i = 0; i < game_nb_rows(g); i++)
        cpt += nb_square_row(g, i, s);
    return cpt;
}

/* ************************************************************************** */
/*                     DIRECTION / COORDINATES FUNCTIONS                      */
/* ************************************************************************** */

uint get_array_index(cgame g, uint i, uint j) {
    return get_array_index_aux(i, j, game_nb_cols(g));
}

uint get_array_index_aux(uint i, uint j, uint nb_cols) { return i * nb_cols + j; }

bool check_coor(cgame g, int i, int j) {
    return i >= 0 && j >= 0 && i < game_nb_rows(g) && j < game_nb_cols(g);
}

coor make_coor(int row, int col) {
    coor tmp;
    tmp.row = row;
    tmp.col = col;
    return tmp;
}

int row_of_coor(coor cor) { return cor.row; }
int col_of_coor(coor cor) { return cor.col; }

coor coor_to_dir(int i, int j, direction dir) {
    coor tmp = direction_to_coor(dir);
    return make_coor(i + row_of_coor(tmp), j + col_of_coor(tmp));
}

coor direction_to_coor(direction dir) {
    switch (dir) {
        case NONE: return make_coor(0, 0);
        case NORTH: return make_coor(-1, 0);
        case SOUTH: return make_coor(1, 0);
        case WEST: return make_coor(0, -1);
        case EAST: return make_coor(0, 1);
        case NORTH_WEST: return make_coor(-1, -1);
        case NORTH_EAST: return make_coor(-1, 1);
        case SOUTH_WEST: return make_coor(1, -1);
        case SOUTH_EAST: return make_coor(1, 1);
        default: return make_coor(0, 0);
    }
}

bool neigh_set_square(game g, uint i, uint j, direction dir, square s) {
    coor new = coor_to_dir(i, j, dir);
    uint new_i = row_of_coor(new);
    uint new_j = col_of_coor(new);

    if (game_is_wrapping(g)) {
        new_i = (new_i + game_nb_rows(g)) % game_nb_rows(g);
        new_j = (new_j + game_nb_cols(g)) % game_nb_cols(g);
    }
    if (check_coor(g, new_i, new_j)) {
        game_set_square(g, new_i, new_j, s);
        return true;
    }
    return false;
}

bool neigh_check_square(cgame g, uint i, uint j, direction dir, square s) {
    coor new = coor_to_dir(i, j, dir);
    uint new_i = row_of_coor(new);
    uint new_j = col_of_coor(new);

    if (game_is_wrapping(g)) {
        new_i = (new_i + game_nb_rows(g)) % game_nb_rows(g);
        new_j = (new_j + game_nb_cols(g)) % game_nb_cols(g);
    }
    if (check_coor(g, new_i, new_j))
        return game_get_square(g, new_i, new_j) == s;
    return false;
}

bool neigh_get_square(cgame g, uint i, uint j, square s, bool diag) {
    if (neigh_check_square(g, i, j, WEST, s)) return true;
    if (neigh_check_square(g, i, j, EAST, s)) return true;
    if (neigh_check_square(g, i, j, SOUTH, s)) return true;
    if (neigh_check_square(g, i, j, NORTH, s)) return true;

    if (diag) {
        if (neigh_check_square(g, i, j, NORTH_WEST, s)) return true;
        if (neigh_check_square(g, i, j, NORTH_EAST, s)) return true;
        if (neigh_check_square(g, i, j, SOUTH_WEST, s)) return true;
        if (neigh_check_square(g, i, j, SOUTH_EAST, s)) return true;
    }
    return false;
}

uint neigh_count(cgame g, uint i, uint j, square s, bool diag) {
    uint cpt = 0;

    if (neigh_check_square(g, i, j, WEST, s)) cpt++;
    if (neigh_check_square(g, i, j, EAST, s)) cpt++;
    if (neigh_check_square(g, i, j, SOUTH, s)) cpt++;
    if (neigh_check_square(g, i, j, NORTH, s)) cpt++;

    if (diag) {
        if (neigh_check_square(g, i, j, NORTH_WEST, s)) cpt++;
        if (neigh_check_square(g, i, j, NORTH_EAST, s)) cpt++;
        if (neigh_check_square(g, i, j, SOUTH_WEST, s)) cpt++;
        if (neigh_check_square(g, i, j, SOUTH_EAST, s)) cpt++;
    }

    return cpt;
}

uint neigh_count_valid(cgame g, int i, int j) {
    if (game_is_wrapping(g))
        return 4;
    uint cpt = 0;

    if (check_coor(g, i - 1, j)) cpt++;
    if (check_coor(g, i, j + 1)) cpt++;
    if (check_coor(g, i + 1, j)) cpt++;
    if (check_coor(g, i, j - 1)) cpt++;

    return cpt;
}

bool neigh_all_get_square(cgame g, uint i, uint j, square s) {
    return neigh_count_valid(g, i, j) == neigh_count(g, i, j, s, false);
}

/* ************************************************************************** */
/*                    CHECK RULES / GAME IS OVER FUNCTIONS                    */
/* ************************************************************************** */

bool check_expected_nb_tents_row(cgame g, uint i) {
    return game_get_current_nb_tents_row(g, i) == game_get_expected_nb_tents_row(g, i);
}

bool check_expected_nb_tents_col(cgame g, uint j) {
    return game_get_current_nb_tents_col(g, j) == game_get_expected_nb_tents_col(g, j);
}

bool check_expected_nb_tents_all(cgame g) {
    return game_get_current_nb_tents_all(g) == game_get_expected_nb_tents_all(g);
}

// Checks rule 1 : no tents are orthogonally adjacent (+ diagonally if diagadj is enabled).
bool check_rule1(cgame g) {
    for (uint i = 0; i < game_nb_rows(g); i++)
        for (uint j = 0; j < game_nb_cols(g); j++)
            if (check_square_tent(g, i, j))
                if (neigh_get_square(g, i, j, TENT, !game_is_diagadj(g)))
                    return false;
    return true;
}

// Checks rule 2 : The number of tents in each row, and in each column, matches the expected numbers given around the sides of the grid.
bool check_rule2(cgame g) {
    // Gross check, only taking numbers into account.
    if (!check_expected_nb_tents_all(g))
        return false;

    // Detailed check, we now verify each row and col one by one.
    for (uint i = 0; i < game_nb_rows(g); i++)
        if (!check_expected_nb_tents_row(g, i))
            return false;

    for (uint j = 0; j < game_nb_cols(g); j++)
        if (!check_expected_nb_tents_col(g, j))
            return false;
    return true;
}

// Checks rule 3 : There are exactly as many tents as trees.
bool check_rule3(cgame g) { return nb_square_all(g, TENT) == nb_square_all(g, TREE); }

// Checks rule 4 : Each tent must be orthogonally adjacent to at least one tree and vice versa.
bool check_rule4(cgame g) {
    for (uint i = 0; i < game_nb_rows(g); i++)
        for (uint j = 0; j < game_nb_cols(g); j++) {
            if (check_square_tree(g, i, j))  // (i, j) tree get a tent on his orthogonally neighbors.
                if (!neigh_get_square(g, i, j, TENT, false))
                    return false;
            if (check_square_tent(g, i, j))  // (i, j) tent get a tree on his orthogonally neighbors.
                if (!neigh_get_square(g, i, j, TREE, false))
                    return false;
        }
    return true;
}

/* ************************************************************************** */
/*                              STACK FUNCTIONS                               */
/* ************************************************************************** */

stack stack_create_empty() {
    // 1/ Allocation of the s structure and content field of s structure
    stack s = malloc(sizeof(struct stack));
    if (!s)
        display_error_and_exit("Not enough memory for \"s\" pointer");

    s->content = malloc(INIT_STACK_SIZE * sizeof(struct move));
    if (!s->content)
        display_error_and_exit("Not enough memory for \"s->content\" pointer");

    // 2/ Seting general parameters of the stack
    s->size = INIT_STACK_SIZE;
    s->next_available_pos = 0;

    return s;
}

bool stack_is_empty(stack s) {
    if (!s)
        display_error_and_exit("s pointer is null");
    // Stack is empty is the available_pos is at 0.
    return s->next_available_pos == 0;
}

move stack_top_value(stack s) {
    if (stack_is_empty(s))
        display_error_and_exit("s pointer is null");

    return s->content[s->next_available_pos - 1];
}

stack stack_push(stack s, move data) {
    if (!s)
        display_error_and_exit("s pointer is null");

    if (s->next_available_pos == s->size) {  // Stack is full, we have to allocate more memory
        s->content = (move *)realloc(s->content, 2 * s->size * sizeof(move));
        if (!s->content)  // Not enough memory for new stack --> end of program
            exit(EXIT_FAILURE);
        s->size = s->size * 2;
    }

    s->content[s->next_available_pos] = data;
    s->next_available_pos = s->next_available_pos + 1;
    return s;
}

stack stack_pop(stack s) {
    if (stack_is_empty(s))
        display_error_and_exit("s pointer is null");

    s->next_available_pos--;
    return s;
}

void stack_dump(stack s) {
    while (!stack_is_empty(s))
        s = stack_pop(s);
}

move make_move(cgame g, uint i, uint j, square s) {
    move new_move;
    new_move.x = i;
    new_move.y = j;
    new_move.old = game_get_square(g, i, j);
    new_move.new = s;
    return new_move;
}

/* ************************************************************************** */
/*                             CREATE RANDOM GAME                             */
/* ************************************************************************** */

static void place_random_tree(game g, uint i, uint j) {
    uint rdm = (rand() % 3) + 1;
    bool tent_placed = true;

    while (tent_placed) {
        if (neigh_check_square(g, i, j, rdm, EMPTY))
            if (neigh_set_square(g, i, j, rdm, TREE))
                tent_placed = false;
        rdm = (rand() % 3) + 1;
    }
}

game generate_random(uint nb_rows, uint nb_cols, uint tree_to_place, bool wrapping, bool diagadj) {
    game g = game_new_empty_ext(nb_rows, nb_cols, wrapping, diagadj);

    srand(time(NULL));
    uint rdm = rand() % (nb_rows + nb_cols);

    while (tree_to_place != nb_square_all(g, TENT)) {
        for (uint i = 0; i < nb_rows; i++) {
            for (uint j = 0; j < nb_cols; j++) {
                if (rdm <= 1 && tree_to_place != nb_square_all(g, TENT)) {
                    if (neigh_count(g, i, j, TENT, !diagadj) == 0) {
                        if (check_square_empty(g, i, j)) {
                            game_set_square(g, i, j, TENT);
                            game_set_expected_nb_tents_row(g, i, game_get_expected_nb_tents_row(g, i) + 1);
                            game_set_expected_nb_tents_col(g, j, game_get_expected_nb_tents_col(g, j) + 1);
                        }
                    }
                }
                rdm = rand() % (nb_rows + nb_cols);
            }
        }
    }

    for (uint i = 0; i < nb_rows; i++)
        for (uint j = 0; j < nb_cols; j++)
            if (check_square_tent(g, i, j)) {
                place_random_tree(g, i, j);
                game_set_square(g, i, j, EMPTY);
            }

    return g;
}