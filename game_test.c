#include "header/game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header/game_aux.h"
#include "header/game_ext.h"
#include "header/game_tools.h"

typedef struct coor {
    uint row, col;
} coor;

typedef enum {
    NONE,
    NORTH,
    SOUTH,
    WEST,
    EAST,
    NORTH_WEST,
    NORTH_EAST,
    SOUTH_WEST,
    SOUTH_EAST
} direction;

// move struct stores the position of a square, its value before it was modified (old) and its current value (new)
typedef struct move {
    uint x;
    uint y;
    square old;
    square new;
} move;

static void make_win_game(game g) {
    game_play_move(g, 0, 0, TENT);
    game_play_move(g, 0, 3, TENT);
    game_play_move(g, 0, 6, TENT);
    game_play_move(g, 2, 0, TENT);
    game_play_move(g, 2, 3, TENT);
    game_play_move(g, 2, 5, TENT);
    game_play_move(g, 2, 7, TENT);
    game_play_move(g, 4, 0, TENT);
    game_play_move(g, 4, 2, TENT);
    game_play_move(g, 4, 4, TENT);
    game_play_move(g, 4, 6, TENT);
    game_play_move(g, 6, 0, TENT);
}

static bool check_square_value(cgame g, uint i, uint j, square s) {
    return game_get_square(g, i, j) == s;
}

static bool check_square_empty(cgame g, uint i, uint j) {
    return check_square_value(g, i, j, EMPTY);
}
static bool check_square_grass(cgame g, uint i, uint j) {
    return check_square_value(g, i, j, GRASS);
}
static bool check_square_tent(cgame g, uint i, uint j) {
    return check_square_value(g, i, j, TENT);
}

static bool check_pos_is_on_game(cgame g, int i, int j) {
    return i >= 0 && j >= 0 && i < game_nb_rows(g) && j < game_nb_cols(g);
}

static uint get_array_index(cgame g, uint i, uint j) {
    if (check_pos_is_on_game(g, i, j))
        return game_nb_cols(g) * i + j;
    return 0;
}

static uint nb_square_row(cgame g, uint i, square s) {
    uint cpt = 0;
    for (uint j = 0; j < game_nb_cols(g); j++)
        if (check_square_value(g, i, j, s))
            cpt++;
    return cpt;
}

static uint nb_square_col(cgame g, uint j, square s) {
    uint cpt = 0;
    for (uint i = 0; i < game_nb_rows(g); i++)
        if (check_square_value(g, i, j, s))
            cpt++;
    return cpt;
}

static uint nb_square_all(cgame g, square s) {
    uint cpt = 0;
    for (uint i = 0; i < game_nb_rows(g); i++)
        for (uint j = 0; j < game_nb_cols(g); j++)
            if (check_square_value(g, i, j, s))
                cpt++;
    return cpt;
}

static coor make_coor(int row, int col) {
    coor tmp;
    tmp.row = row;
    tmp.col = col;
    return tmp;
}

static int row_of_coor(coor cor) {
    return cor.row;
}
static int col_of_coor(coor cor) {
    return cor.col;
}

static coor direction_to_coor(direction dir) {
    switch (dir) {
        case NONE:
            return make_coor(0, 0);
        case NORTH:
            return make_coor(-1, 0);
        case SOUTH:
            return make_coor(1, 0);
        case WEST:
            return make_coor(0, -1);
        case EAST:
            return make_coor(0, 1);
        case NORTH_WEST:
            return make_coor(-1, -1);
        case NORTH_EAST:
            return make_coor(-1, 1);
        case SOUTH_WEST:
            return make_coor(1, -1);
        case SOUTH_EAST:
            return make_coor(1, 1);
        default:
            return make_coor(0, 0);
    }
}

static coor coor_to_dir(int i, int j, direction dir) {
    coor gap = direction_to_coor(dir);
    int x = row_of_coor(gap), y = col_of_coor(gap);

    return make_coor(i + x, j + y);
}

game test_game() {
    return game_default();
}
game test_game_solve() {
    return game_default_solution();
}

bool test_get_current_nb_tents_all() {
    game g = test_game();

    if (nb_square_all(g, TENT) != game_get_current_nb_tents_all(g))
        return false;

    make_win_game(g);
    if (nb_square_all(g, TENT) != game_get_current_nb_tents_all(g))
        return false;

    game_delete(g);
    return true;
}

bool test_get_current_nb_tents_col() {
    game g = test_game();

    for (uint j = 0; j < game_nb_cols(g); j++)
        if (nb_square_col(g, j, TENT) != game_get_current_nb_tents_col(g, j))
            return false;

    make_win_game(g);
    for (uint j = 0; j < game_nb_cols(g); j++)
        if (nb_square_col(g, j, TENT) != game_get_current_nb_tents_col(g, j))
            return false;

    game_delete(g);
    return true;
}

bool test_get_current_nb_tents_row() {
    game g = test_game();

    for (uint i = 0; i < game_nb_rows(g); i++)
        if (nb_square_row(g, i, TENT) != game_get_current_nb_tents_row(g, i))
            return false;

    make_win_game(g);
    for (uint i = 0; i < game_nb_rows(g); i++)
        if (nb_square_row(g, i, TENT) != game_get_current_nb_tents_row(g, i))
            return false;

    game_delete(g);
    return true;
}

bool test_get_expected_nb_tents_all() {
    game g = game_new_empty();
    if (game_get_expected_nb_tents_all(g) != 0)
        return false;
    game_delete(g);

    g = test_game();
    if (game_get_expected_nb_tents_all(g) != 12)
        return false;
    game_delete(g);

    g = test_game_solve();
    if (game_get_expected_nb_tents_all(g) != 12)
        return false;

    game_delete(g);
    return true;
}

bool test_get_expected_nb_tents_col() {
    game g = test_game();
    uint col[] = {4, 0, 1, 2, 1, 1, 2, 1};

    for (uint j = 0; j < game_nb_cols(g); j++)
        if (game_get_expected_nb_tents_col(g, j) != col[j])
            return false;

    make_win_game(g);

    for (uint j = 0; j < game_nb_cols(g); j++)
        if (game_get_expected_nb_tents_col(g, j) != col[j])
            return false;

    game_delete(g);
    g = game_new_empty();

    for (uint j = 0; j < game_nb_cols(g); j++)
        if (game_get_expected_nb_tents_col(g, j) != 0)
            return false;

    game_delete(g);
    return true;
}

bool test_get_expected_nb_tents_row() {
    game g = test_game();
    uint row[] = {3, 0, 4, 0, 4, 0, 1, 0};

    for (uint i = 0; i < game_nb_rows(g); i++)
        if (game_get_expected_nb_tents_row(g, i) != row[i])
            return false;

    make_win_game(g);

    for (uint i = 0; i < game_nb_rows(g); i++)
        if (game_get_expected_nb_tents_row(g, i) != row[i])
            return false;

    game_delete(g);
    g = game_new_empty();

    for (uint i = 0; i < game_nb_rows(g); i++)
        if (game_get_expected_nb_tents_row(g, i) != 0)
            return false;

    game_delete(g);
    return true;
}

bool test_get_square() {
    game g = game_new_empty();

    for (uint i = 0; i < DEFAULT_SIZE; i++) {
        for (uint j = 0; j < DEFAULT_SIZE; j++) {
            if (game_get_square(g, i, j) != EMPTY)
                return false;
        }
    }

    game_delete(g);

    g = test_game();
    square expected_squares[] = {
        EMPTY, EMPTY, EMPTY, EMPTY, TREE, TREE, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};

    for (uint i = 0; i < DEFAULT_SIZE; i++) {
        for (uint j = 0; j < DEFAULT_SIZE; j++) {
            if (game_get_square(g, i, j) !=
                expected_squares[DEFAULT_SIZE * i + j])
                return false;
        }
    }

    game_delete(g);
    return true;
}

bool test_game_print() {
    game g = game_new_empty();
    game_print(g);
    game_delete(g);

    g = game_default();
    game_print(g);
    game_delete(g);

    g = game_default_solution();
    game_print(g);
    game_delete(g);

    return true;
}

// Illegal move
bool test_check_move_0() {
    game g = test_game();

    if (game_check_move(g, 0, 0, TREE) != ILLEGAL)
        return false;
    if (game_check_move(g, 1, 0, TENT) != ILLEGAL)
        return false;
    if (game_check_move(g, 1, 0, GRASS) != ILLEGAL)
        return false;
    if (game_check_move(g, 1, 0, EMPTY) != ILLEGAL)
        return false;

    game_delete(g);
    return true;
}

// Losing move tent with diagadj
bool test_check_move_1() {
    square sq[] = {EMPTY, EMPTY, EMPTY, EMPTY, TREE, TREE, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    uint row[] = {3, 0, 4, 0, 4, 1, 1, 0};
    uint col[] = {4, 0, 1, 2, 1, 1, 2, 1};

    game g = game_new_ext(8, 8, sq, row, col, false, true);
    game_set_square(g, 4, 4, TENT);

    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, NORTH)), col_of_coor(coor_to_dir(4, 4, NORTH)), TENT) != LOSING)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, SOUTH)), col_of_coor(coor_to_dir(4, 4, SOUTH)), TENT) != ILLEGAL)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, EAST)), col_of_coor(coor_to_dir(4, 4, EAST)), TENT) != LOSING)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, WEST)), col_of_coor(coor_to_dir(4, 4, WEST)), TENT) != LOSING)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, NORTH_EAST)), col_of_coor(coor_to_dir(4, 4, NORTH_EAST)), TENT) != ILLEGAL)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, NORTH_WEST)), col_of_coor(coor_to_dir(4, 4, NORTH_WEST)), TENT) != LOSING)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, SOUTH_EAST)), col_of_coor(coor_to_dir(4, 4, SOUTH_EAST)), TENT) != REGULAR)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, SOUTH_WEST)), col_of_coor(coor_to_dir(4, 4, SOUTH_WEST)), TENT) != REGULAR)
        return false;

    game_delete(g);
    return true;
}

// Losing move tent without diagadj
bool test_check_move_2() {
    square sq[] = {EMPTY, EMPTY, EMPTY, EMPTY, TREE, TREE, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    uint row[] = {3, 0, 4, 0, 4, 1, 1, 0};
    uint col[] = {4, 0, 1, 2, 1, 1, 2, 1};

    game g = game_new_ext(8, 8, sq, row, col, false, false);
    game_set_square(g, 4, 4, TENT);

    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, NORTH)), col_of_coor(coor_to_dir(4, 4, NORTH)), TENT) != LOSING)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, SOUTH)), col_of_coor(coor_to_dir(4, 4, SOUTH)), TENT) != ILLEGAL)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, EAST)), col_of_coor(coor_to_dir(4, 4, EAST)), TENT) != LOSING)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, WEST)), col_of_coor(coor_to_dir(4, 4, WEST)), TENT) != LOSING)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, NORTH_EAST)), col_of_coor(coor_to_dir(4, 4, NORTH_EAST)), TENT) != ILLEGAL)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, NORTH_WEST)), col_of_coor(coor_to_dir(4, 4, NORTH_WEST)), TENT) != LOSING)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, SOUTH_EAST)), col_of_coor(coor_to_dir(4, 4, SOUTH_EAST)), TENT) != LOSING)
        return false;
    if (game_check_move(g, row_of_coor(coor_to_dir(4, 4, SOUTH_WEST)), col_of_coor(coor_to_dir(4, 4, SOUTH_WEST)), TENT) != LOSING)
        return false;

    game_delete(g);
    return true;
}

// Losing move tent no get tree
bool test_check_move_3() {
    game g = test_game();

    if (game_check_move(g, 0, 0, TENT) != REGULAR)
        return false;
    if (game_check_move(g, 7, 7, TENT) != LOSING)
        return false;
    if (game_check_move(g, 0, 7, TENT) != REGULAR)
        return false;
    if (game_check_move(g, 0, 1, TENT) != LOSING)
        return false;

    game_delete(g);
    return true;
}

// Losing move tent current >= expected
bool test_check_move_4() {
    game g = test_game();

    if (game_check_move(g, 1, 1, TENT) != LOSING)
        return false;
    if (game_check_move(g, 5, 7, TENT) != LOSING)
        return false;

    game_play_move(g, 0, 0, TENT);
    game_play_move(g, 0, 1, TENT);
    game_play_move(g, 0, 2, TENT);

    if (game_check_move(g, 0, 6, TENT) != LOSING)
        return false;
    if (game_check_move(g, 2, 2, TENT) != LOSING)
        return false;

    game_delete(g);
    return true;
}

// Losing move grass empty tent row
bool test_check_move_5() {
    game g = test_game();

    game_play_move(g, 4, 0, GRASS);
    if (game_check_move(g, 4, 2, GRASS) != REGULAR)
        return false;
    game_play_move(g, 4, 2, GRASS);

    if (game_check_move(g, 4, 3, GRASS) != REGULAR)
        return false;
    game_play_move(g, 4, 3, GRASS);

    if (game_check_move(g, 4, 4, GRASS) != LOSING)
        return false;

    game_delete(g);
    return true;
}

// Losing move grass empty tent col
bool test_check_move_6() {
    game g = test_game();

    if (game_check_move(g, 0, 0, GRASS) != LOSING)
        return false;

    game_delete(g);
    return true;
}

// Regular move
bool test_check_move_7() {
    game g = test_game();

    if (game_check_move(g, 0, 0, TENT) != REGULAR)
        return false;
    if (game_check_move(g, 0, 3, TENT) != REGULAR)
        return false;
    if (game_check_move(g, 0, 6, TENT) != REGULAR)
        return false;
    if (game_check_move(g, 2, 0, TENT) != REGULAR)
        return false;
    if (game_check_move(g, 2, 3, TENT) != REGULAR)
        return false;
    if (game_check_move(g, 2, 5, TENT) != REGULAR)
        return false;
    if (game_check_move(g, 2, 7, TENT) != REGULAR)
        return false;
    if (game_check_move(g, 4, 0, TENT) != REGULAR)
        return false;
    if (game_check_move(g, 4, 2, TENT) != REGULAR)
        return false;
    if (game_check_move(g, 4, 4, TENT) != REGULAR)
        return false;
    if (game_check_move(g, 4, 6, TENT) != REGULAR)
        return false;

    game_delete(g);
    return true;
}

bool test_check_move_8() {
    square sq[] = {EMPTY, EMPTY, EMPTY, EMPTY, TREE, TREE, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    uint row[] = {3, 0, 4, 0, 4, 1, 1, 0};
    uint col[] = {4, 0, 1, 2, 1, 1, 2, 1};

    game g = game_new_ext(8, 8, sq, row, col, true, false);
    game_play_move(g, 2, 0, GRASS);
    game_play_move(g, 4, 0, GRASS);
    game_play_move(g, 3, 1, GRASS);

    if (game_check_move(g, 3, 7, GRASS) != LOSING)
        return false;

    game_delete(g);
    return true;
}

bool test_copy() {
    game g = test_game();
    game_play_move(g, 0, 0, GRASS);
    game cg = game_copy(g);

    if (!game_equal(g, cg))
        return false;

    game_delete(g);
    game_delete(cg);

    g = game_new_empty_ext(3, 5, true, true);
    cg = game_copy(g);

    if (!game_equal(g, cg))
        return false;

    game_delete(g);
    game_delete(cg);
    return true;
}

bool test_delete() {
    game_delete(game_default());
    game_delete(game_default_solution());

    return true;
}

// Game and only game is equal
bool test_equal_0() {
    game g = test_game();
    game cp = game_copy(g);

    if (!game_equal(g, cp))
        return false;

    game_play_move(g, 0, 0, GRASS);
    if (game_equal(g, cp))
        return false;
    game_play_move(g, 0, 0, EMPTY);
    if (!game_equal(g, cp))
        return false;

    game_set_expected_nb_tents_col(g, 0, 2);
    if (game_equal(g, cp))
        return false;
    game_set_expected_nb_tents_col(g, 0, 4);
    if (!game_equal(g, cp))
        return false;

    game_set_expected_nb_tents_row(g, 0, 2);
    if (game_equal(g, cp))
        return false;
    game_set_expected_nb_tents_row(g, 0, 3);
    if (!game_equal(g, cp))
        return false;

    game_delete(g);
    game_delete(cp);
    return true;
}

// Settings of game is equal
bool test_equal_1() {
    square sq[] = {TREE, EMPTY, TENT, EMPTY, EMPTY, EMPTY};
    uint row[] = {1, 0};
    uint col[] = {0, 0, 1};

    game g = game_new_ext(2, 3, sq, row, col, false, false);
    game cp = game_copy(g);

    if (!game_equal(g, cp))
        return false;

    game_delete(g);
    g = game_new_ext(2, 3, sq, row, col, false, true);
    if (game_equal(g, cp))
        return false;

    game_delete(g);
    g = game_new_ext(2, 3, sq, row, col, true, false);
    if (game_equal(g, cp))
        return false;

    game_delete(g);
    g = game_new_ext(3, 2, sq, row, col, false, false);
    if (game_equal(g, cp))
        return false;

    game_delete(g);
    game_delete(cp);
    return true;
}

bool test_fill_grass_col() {
    game g = test_game();
    game g_win = test_game_solve();

    make_win_game(g);

    for (uint i = 0; i < game_nb_rows(g); i++) {
        game_fill_grass_col(g, i);
        for (uint j = i + 1; j < game_nb_cols(g); j++) {
            if (game_get_square(g, i, j) == GRASS)
                return false;
        }
    }

    if (!game_equal(g, g_win))
        return false;
    game_delete(g);
    game_delete(g_win);

    g = game_new_empty_ext(3, 3, false, false);

    game_fill_grass_col(g, 0);

    if (nb_square_col(g, 0, GRASS) != 3)
        return false;
    game_undo(g);
    if (nb_square_col(g, 0, GRASS) != 2)
        return false;
    game_undo(g);
    if (nb_square_col(g, 0, GRASS) != 1)
        return false;

    game_delete(g);

    return true;
}

bool test_fill_grass_row() {
    game g = test_game();
    game g_win = test_game_solve();

    make_win_game(g);

    for (uint i = 0; i < game_nb_rows(g); i++) {
        game_fill_grass_row(g, i);
    }

    if (!game_equal(g, g_win))
        return false;
    game_delete(g);
    game_delete(g_win);

    g = game_new_empty_ext(3, 3, false, false);

    game_fill_grass_row(g, 0);

    if (nb_square_row(g, 0, GRASS) != 3)
        return false;
    game_undo(g);
    if (nb_square_row(g, 0, GRASS) != 2)
        return false;
    game_undo(g);
    if (nb_square_row(g, 0, GRASS) != 1)
        return false;

    game_delete(g);

    return true;
}

bool test_game_default() {
    game_delete(game_default());
    return true;
}

bool test_game_default_solution() {
    game_delete(game_default_solution());
    return true;
}

// Losing game
bool test_is_over_0() {
    game g = test_game();
    if (game_is_over(g))
        return false;
    game_delete(g);

    return true;
}

// Rule 1 (v1)
bool test_is_over_1() {
    square sq[] = {TREE, TENT, EMPTY, TENT, TREE, EMPTY, EMPTY, EMPTY, EMPTY};
    uint row[] = {1, 1, 0};
    uint col[] = {1, 1, 0};

    game g = game_new_ext(3, 3, sq, row, col, false, false);

    if (game_is_over(g))
        return false;
    game_delete(g);

    return true;
}

// Rule 1 with diagadj
bool test_is_over_2() {
    square sq[] = {TREE, TENT, EMPTY, TENT, TREE, EMPTY, EMPTY, EMPTY, EMPTY};
    uint row[] = {1, 1, 0};
    uint col[] = {1, 1, 0};

    game g = game_new_ext(3, 3, sq, row, col, false, true);

    if (!game_is_over(g))
        return false;
    game_delete(g);

    return true;
}

// Rule 1 with wrapping
bool test_is_over_3() {
    square sq[] = {TREE, EMPTY, TENT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    uint row[] = {1, 0, 0};
    uint col[] = {0, 0, 1};

    game g = game_new_ext(3, 3, sq, row, col, false, false);
    if (game_is_over(g))
        return false;
    game_delete(g);

    g = game_new_ext(3, 3, sq, row, col, true, false);
    if (!game_is_over(g))
        return false;
    game_delete(g);

    return true;
}

// Rule 1 with wrapping && diagadj
bool test_is_over_4() {
    square sq[] = {TENT, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TENT};
    uint row[] = {1, 0, 1};
    uint col[] = {1, 0, 1};

    game g = game_new_ext(3, 3, sq, row, col, false, false);
    if (game_is_over(g))
        return false;
    game_delete(g);

    g = game_new_ext(3, 3, sq, row, col, true, false);
    if (game_is_over(g))
        return false;
    game_delete(g);

    g = game_new_ext(3, 3, sq, row, col, false, true);
    if (game_is_over(g))
        return false;
    game_delete(g);

    g = game_new_ext(3, 3, sq, row, col, true, true);
    if (!game_is_over(g))
        return false;
    game_delete(g);

    return true;
}

// Rule 2
bool test_is_over_5() {
    game g = test_game_solve();

    game_play_move(g, 1, 1, TENT);
    if (game_is_over(g))
        return false;
    game_play_move(g, 1, 1, EMPTY);

    game_play_move(g, 6, 4, TENT);
    if (game_is_over(g))
        return false;

    game_delete(g);
    return true;
}

// Rule 3
bool test_is_over_6() {
    game g = test_game_solve();

    game_play_move(g, 1, 1, TENT);
    if (game_is_over(g))
        return false;
    game_play_move(g, 1, 1, EMPTY);

    game_set_square(g, 1, 0, GRASS);
    if (game_is_over(g))
        return false;

    game_delete(g);
    return true;
}

// Rule 4
bool test_is_over_7() {
    game g = test_game_solve();

    game_play_move(g, 0, 0, EMPTY);
    game_play_move(g, 6, 4, TENT);
    if (game_is_over(g))
        return false;

    game_delete(g);
    return true;
}

// Winning game
bool test_is_over_8() {
    game g = test_game();

    make_win_game(g);
    if (!game_is_over(g))
        return false;
    game_delete(g);

    g = test_game_solve();
    if (!game_is_over(g))
        return false;

    game_delete(g);
    return true;
}

bool test_new() {
    square sq[] = {EMPTY, EMPTY, EMPTY, EMPTY, TREE, TREE, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
    uint row[] = {3, 0, 4, 0, 4, 1, 1, 0};
    uint col[] = {4, 0, 1, 2, 1, 1, 2, 1};

    game g = game_new(sq, row, col);

    if (game_nb_rows(g) != DEFAULT_SIZE)
        return false;
    if (game_nb_cols(g) != DEFAULT_SIZE)
        return false;
    if (game_is_diagadj(g))
        return false;
    if (game_is_wrapping(g))
        return false;

    for (uint i = 0; i < game_nb_rows(g); i++) {
        for (uint j = 0; j < game_nb_cols(g); j++) {
            if (game_get_square(g, i, j) != sq[get_array_index(g, i, j)])
                return false;

            if (i == 0)
                if (game_get_expected_nb_tents_col(g, j) != col[j])
                    return false;
        }
        if (game_get_expected_nb_tents_row(g, i) != row[i])
            return false;
    }

    game_delete(g);
    return true;
}

bool test_new_empty() {
    game g = game_new_empty();

    if (game_nb_rows(g) != DEFAULT_SIZE)
        return false;
    if (game_nb_cols(g) != DEFAULT_SIZE)
        return false;
    if (game_is_diagadj(g))
        return false;
    if (game_is_wrapping(g))
        return false;

    for (uint i = 0; i < game_nb_rows(g); i++) {
        for (uint j = 0; j < game_nb_cols(g); j++) {
            if (game_get_square(g, i, j) != EMPTY)
                return false;

            if (i == 0)
                if (game_get_expected_nb_tents_col(g, j) != 0)
                    return false;
        }
        if (game_get_expected_nb_tents_row(g, i) != 0)
            return false;
    }

    game_delete(g);
    return true;
}

bool test_play_move() {
    game g = test_game();

    game_play_move(g, 0, 0, TENT);
    if (game_get_square(g, 0, 0) != TENT)
        return false;
    // if (stack_is_empty(g->action->undo))
    //     return false;

    game_delete(g);
    return true;
}

bool test_restart() {
    game g = test_game();
    game wg = test_game_solve();

    game_restart(wg);
    if (!game_equal(g, wg))
        return false;
    game_delete(g);
    game_delete(wg);

    square sq[] = {EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, EMPTY};
    uint row[] = {1, 0, 1};
    uint col[] = {1, 0, 1};

    g = game_new_ext(3, 3, sq, row, col, true, true);
    game cg = game_copy(g);
    game_play_move(cg, 0, 1, GRASS);
    game_undo(cg);
    game_play_move(cg, 0, 1, GRASS);

    game_restart(cg);
    if (!game_equal(g, cg))
        return false;

    game_undo(cg);
    game_redo(cg);

    if (game_get_square(cg, 0, 1) == GRASS)
        return false;

    game_delete(cg);
    game_delete(g);

    return true;
}

bool test_set_expected_nb_tents_col() {
    game g = game_new_empty();
    uint col[] = {4, 0, 1, 2, 1, 1, 2, 1};

    for (uint j = 0; j < DEFAULT_SIZE; j++)
        game_set_expected_nb_tents_col(g, j, col[j]);

    for (uint j = 0; j < DEFAULT_SIZE; j++) {
        if (game_get_expected_nb_tents_col(g, j) != col[j])
            return false;
        if (game_get_expected_nb_tents_row(g, j) != 0)
            return false;
    }

    game_delete(g);
    return true;
}

bool test_set_expected_nb_tents_row() {
    game g = game_new_empty();
    uint row[] = {3, 0, 4, 0, 4, 0, 1, 0};

    for (uint i = 0; i < DEFAULT_SIZE; i++)
        game_set_expected_nb_tents_row(g, i, row[i]);

    for (uint j = 0; j < DEFAULT_SIZE; j++) {
        if (game_get_expected_nb_tents_row(g, j) != row[j])
            return false;
        if (game_get_expected_nb_tents_col(g, j) != 0)
            return false;
    }

    game_delete(g);
    return true;
}

bool test_set_square() {
    game g = game_new_empty();

    for (uint i = 0; i < game_nb_rows(g); i++) {
        for (uint j = 0; j < game_nb_cols(g); j++) {
            square rdm_square = rand() % 3;
            game_set_square(g, i, j, rdm_square);
            if (game_get_square(g, i, j) != rdm_square)
                return false;
        }
    }

    game_delete(g);
    return true;
}

bool test_nb_rows() {
    square sq[] = {EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY};
    uint row[] = {1, 0};
    uint col[] = {1, 0, 1};

    game g = game_new_ext(2, 3, sq, row, col, true, true);

    if (game_nb_rows(g) != 2)
        return false;
    if (game_nb_rows(g) == 3)
        return false;

    game_delete(g);
    return true;
}

bool test_nb_cols() {
    square sq[] = {EMPTY, EMPTY, TREE, EMPTY, EMPTY, EMPTY};
    uint row[] = {1, 0, 3};
    uint col[] = {1, 0};

    game g = game_new_ext(2, 3, sq, row, col, true, true);

    if (game_nb_cols(g) != 3)
        return false;
    if (game_nb_cols(g) == 2)
        return false;

    game_delete(g);
    return true;
}

bool test_new_empty_ext() {
    game g = game_new_empty_ext(3, 7, true, false);

    if (game_nb_rows(g) != 3)
        return false;
    if (game_nb_cols(g) != 7)
        return false;
    if (game_is_diagadj(g))
        return false;
    if (!game_is_wrapping(g))
        return false;

    for (uint i = 0; i < game_nb_rows(g); i++) {
        for (uint j = 0; j < game_nb_cols(g); j++) {
            if (game_get_square(g, i, j) != EMPTY)
                return false;

            if (i == 0)
                if (game_get_expected_nb_tents_col(g, j) != 0)
                    return false;
        }
        if (game_get_expected_nb_tents_row(g, i) != 0)
            return false;
    }

    game_delete(g);
    return true;
}

bool test_new_ext() {
    square sq[] = {TENT, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TENT, EMPTY, EMPTY, EMPTY};
    uint row[] = {1, 0, 1};
    uint col[] = {1, 0, 1, 1};

    game g = game_new_ext(3, 4, sq, row, col, false, true);

    if (!game_is_diagadj(g))
        return false;
    if (game_is_wrapping(g))
        return false;

    for (uint i = 0; i < game_nb_rows(g); i++) {
        for (uint j = 0; j < game_nb_cols(g); j++) {
            if (game_get_square(g, i, j) != sq[get_array_index(g, i, j)])
                return false;

            if (i == 0)
                if (game_get_expected_nb_tents_col(g, j) != col[j])
                    return false;
        }
        if (game_get_expected_nb_tents_row(g, i) != row[i])
            return false;
    }

    game_delete(g);
    return true;
}

bool test_is_wrapping() {
    square sq[] = {TENT, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TENT, EMPTY, EMPTY, EMPTY};
    uint row[] = {1, 0, 1};
    uint col[] = {1, 0, 1, 1};

    game g = game_new_ext(3, 4, sq, row, col, false, true);
    if (game_is_wrapping(g))
        return false;

    game_delete(g);
    g = game_new_ext(3, 4, sq, row, col, true, true);
    if (!game_is_wrapping(g))
        return false;

    game_delete(g);
    return true;
}

bool test_is_diagadj() {
    square sq[] = {TENT, EMPTY, TREE, EMPTY, EMPTY, EMPTY, TREE, EMPTY, TENT, EMPTY, EMPTY, EMPTY};
    uint row[] = {1, 0, 1};
    uint col[] = {1, 0, 1, 1};

    game g = game_new_ext(3, 4, sq, row, col, false, true);
    if (!game_is_diagadj(g))
        return false;

    game_delete(g);
    g = game_new_ext(3, 4, sq, row, col, true, false);
    if (game_is_diagadj(g))
        return false;

    game_delete(g);
    return true;
}

bool test_undo() {
    square sq[] = {EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY, GRASS, EMPTY};
    uint row[] = {1, 0, 1};
    uint col[] = {1, 0, 1};
    game g = game_new_ext(3, 3, sq, row, col, false, false);

    game_play_move(g, 0, 0, TENT);
    game_play_move(g, 0, 2, TENT);

    game_undo(g);

    if (!check_square_empty(g, 0, 2))
        return false;

    if (!check_square_tent(g, 0, 0))
        return false;

    game_undo(g);

    if (!check_square_empty(g, 0, 2))
        return false;

    if (!check_square_empty(g, 0, 0))
        return false;

    game_play_move(g, 2, 1, EMPTY);
    game_undo(g);

    if (!check_square_grass(g, 2, 1))
        return false;

    game_delete(g);
    return true;
}

bool test_redo() {
    square sq[] = {EMPTY, EMPTY, EMPTY, TREE, EMPTY, TREE, EMPTY, GRASS, EMPTY};
    uint row[] = {1, 0, 1};
    uint col[] = {1, 0, 1};
    game g = game_new_ext(3, 3, sq, row, col, false, false);

    game_play_move(g, 0, 0, TENT);
    game_play_move(g, 0, 2, TENT);
    game_undo(g);
    game_redo(g);

    if (!check_square_tent(g, 0, 2))
        return false;

    game_set_square(g, 0, 0, EMPTY);

    game_play_move(g, 0, 0, TENT);
    game_undo(g);
    game_play_move(g, 0, 2, TENT);
    game_redo(g);

    if (!check_square_empty(g, 0, 0))
        return false;

    game_delete(g);
    return true;
}

bool test_load() {
    game gd = game_default();
    game gds = game_default_solution();

    game g = game_load("../data/game_default.tnt");
    game gs = game_load("../data/game_default_solution.tnt");

    if (!game_equal(gd, g))
        return false;
    if (!game_equal(gds, gs))
        return false;

    game_delete(gd);
    game_delete(gds);
    game_delete(g);
    game_delete(gs);

    g = game_load("../data/game_2x4.tnt");
    if (game_nb_rows(g) != 2)
        return false;
    if (game_nb_cols(g) != 4)
        return false;
    if (game_is_wrapping(g))
        return false;
    if (game_is_diagadj(g))
        return false;

    game_delete(g);
    g = game_load("../data/game_3x3d.tnt");
    if (game_is_wrapping(g))
        return false;
    if (!game_is_diagadj(g))
        return false;

    game_delete(g);
    g = game_load("../data/game_3x3w.tnt");
    if (!game_is_wrapping(g))
        return false;
    if (game_is_diagadj(g))
        return false;

    game_delete(g);
    g = game_load("../data/game_3x3wd.tnt");
    if (!game_is_wrapping(g))
        return false;
    if (!game_is_diagadj(g))
        return false;

    game_delete(g);
    return true;
}

bool test_save() {
    game gd = game_default();
    game gds = game_default_solution();

    game_save(gd, "../data/test1.tnt");
    game_save(gds, "../data/test2.tnt");

    game g = game_load("../data/test1.tnt");
    game gs = game_load("../data/test2.tnt");

    if (!game_equal(gd, g))
        return false;

    if (!game_equal(gds, gs))
        return false;

    game_delete(gd);
    game_delete(gds);
    game_delete(g);
    game_delete(gs);

    return true;
}

bool test_solve() {
    game g = game_default();
    game_solve(g);
    if (!game_is_over(g))
        return false;
    game_delete(g);

    g = game_load("../data/game_3x3wd.tnt");
    game_solve(g);
    if (!game_is_over(g))
        return false;
    game_delete(g);

    return true;
}

bool test_nb_sol() {
    game g = game_load("../data/game_nb_sol4.tnt");
    if (game_nb_solutions(g) != 4)
        return false;
    game_delete(g);

    return true;
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        char *arg = argv[1];
        bool ok = false;

        if (strcmp("new", arg) == 0)
            ok = test_new();
        else if (strcmp("new_empty", arg) == 0)
            ok = test_new_empty();
        else if (strcmp("play_move", arg) == 0)
            ok = test_play_move();
        else if (strcmp("restart", arg) == 0)
            ok = test_restart();
        else if (strcmp("set_expected_nb_tents_col", arg) == 0)
            ok = test_set_expected_nb_tents_col();
        else if (strcmp("set_expected_nb_tents_row", arg) == 0)
            ok = test_set_expected_nb_tents_row();
        else if (strcmp("set_square", arg) == 0)
            ok = test_set_square();
        else if (strcmp("get_current_nb_tents_all", arg) == 0)
            ok = test_get_current_nb_tents_all();
        else if (strcmp("get_current_nb_tents_col", arg) == 0)
            ok = test_get_current_nb_tents_col();
        else if (strcmp("get_current_nb_tents_row", arg) == 0)
            ok = test_get_current_nb_tents_row();
        else if (strcmp("get_expected_nb_tents_all", arg) == 0)
            ok = test_get_expected_nb_tents_all();
        else if (strcmp("get_expected_nb_tents_col", arg) == 0)
            ok = test_get_expected_nb_tents_col();
        else if (strcmp("get_expected_nb_tents_row", arg) == 0)
            ok = test_get_expected_nb_tents_row();
        else if (strcmp("get_square", arg) == 0)
            ok = test_get_square();
        else if (strcmp("game_print", arg) == 0)
            ok = test_game_print();
        else if (strcmp("copy", arg) == 0)
            ok = test_copy();
        else if (strcmp("delete", arg) == 0)
            ok = test_delete();
        else if (strcmp("equal_0", arg) == 0)
            ok = test_equal_0();
        else if (strcmp("equal_1", arg) == 0)
            ok = test_equal_1();
        else if (strcmp("fill_grass_col", arg) == 0)
            ok = test_fill_grass_col();
        else if (strcmp("fill_grass_row", arg) == 0)
            ok = test_fill_grass_row();
        else if (strcmp("game_default", arg) == 0)
            ok = test_game_default();
        else if (strcmp("game_default_solution", arg) == 0)
            ok = test_game_default_solution();
        else if (strcmp("check_move_0", arg) == 0)
            ok = test_check_move_0();
        else if (strcmp("check_move_1", arg) == 0)
            ok = test_check_move_1();
        else if (strcmp("check_move_2", arg) == 0)
            ok = test_check_move_2();
        else if (strcmp("check_move_3", arg) == 0)
            ok = test_check_move_3();
        else if (strcmp("check_move_4", arg) == 0)
            ok = test_check_move_4();
        else if (strcmp("check_move_5", arg) == 0)
            ok = test_check_move_5();
        else if (strcmp("check_move_6", arg) == 0)
            ok = test_check_move_6();
        else if (strcmp("check_move_7", arg) == 0)
            ok = test_check_move_7();
        else if (strcmp("check_move_8", arg) == 0)
            ok = test_check_move_8();
        else if (strcmp("is_over_0", arg) == 0)
            ok = test_is_over_0();
        else if (strcmp("is_over_1", arg) == 0)
            ok = test_is_over_1();
        else if (strcmp("is_over_2", arg) == 0)
            ok = test_is_over_2();
        else if (strcmp("is_over_3", arg) == 0)
            ok = test_is_over_3();
        else if (strcmp("is_over_4", arg) == 0)
            ok = test_is_over_4();
        else if (strcmp("is_over_5", arg) == 0)
            ok = test_is_over_5();
        else if (strcmp("is_over_6", arg) == 0)
            ok = test_is_over_6();
        else if (strcmp("is_over_7", arg) == 0)
            ok = test_is_over_7();
        else if (strcmp("is_over_8", arg) == 0)
            ok = test_is_over_8();
        else if (strcmp("nb_rows", arg) == 0)
            ok = test_nb_rows();
        else if (strcmp("nb_cols", arg) == 0)
            ok = test_nb_cols();
        else if (strcmp("new_empty_ext", arg) == 0)
            ok = test_new_empty_ext();
        else if (strcmp("new_ext", arg) == 0)
            ok = test_new_ext();
        else if (strcmp("is_wrapping", arg) == 0)
            ok = test_is_wrapping();
        else if (strcmp("is_diagadj", arg) == 0)
            ok = test_is_diagadj();
        else if (strcmp("undo", arg) == 0)
            ok = test_undo();
        else if (strcmp("redo", arg) == 0)
            ok = test_redo();
        else if (strcmp("load", arg) == 0)
            ok = test_load();
        else if (strcmp("save", arg) == 0)
            ok = test_save();
        else if (strcmp("solve", arg) == 0)
            ok = test_solve();
        else if (strcmp("nb_sol", arg) == 0)
            ok = test_nb_sol();
        else {
            fprintf(stderr, "Error: test \"%s\" not found!\n", arg);
            exit(EXIT_FAILURE);
        }

        if (!ok)
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    } else {
        exit(EXIT_FAILURE);
    }
}