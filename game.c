#include "header/game.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "header/game_ext.h"
#include "header/private.h"

/* ************************************************************************** */
/*                              BASICS FUNCTIONS                              */
/* ************************************************************************** */

game game_new(square *squares, uint *nb_tents_row, uint *nb_tents_col) {
    return game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, squares, nb_tents_row, nb_tents_col, false, false);
}

game game_new_empty(void) {
    return game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false, false);
}

game game_copy(cgame g) {
    assert(g);
    return game_new_ext(game_nb_rows(g), game_nb_cols(g), g->squares, g->nb_tents_row, g->nb_tents_col, game_is_wrapping(g), game_is_diagadj(g));
}

bool game_equal(cgame g1, cgame g2) {
    assert(g1 && g1);

    for (uint i = 0; i < game_nb_rows(g1); i++) {
        for (uint j = 0; j < game_nb_cols(g1); j++) {
            // Checking that the expected numbers of tents per row matches between both games.
            if (i == 0)
                if (game_get_expected_nb_tents_col(g1, i) != game_get_expected_nb_tents_col(g2, i))
                    return false;
            // Checking that each square is equal between both games.
            if (game_get_square(g1, i, j) != game_get_square(g2, i, j))
                return false;
        }
        // Checking that the expected numbers of tents per row matches between both games.
        if (game_get_expected_nb_tents_row(g1, i) != game_get_expected_nb_tents_row(g2, i))
            return false;
    }
    // Lastly, testing the remaining parameters
    if (game_nb_rows(g1) != game_nb_rows(g2)) return false;
    if (game_nb_cols(g1) != game_nb_cols(g2)) return false;
    if (game_is_wrapping(g1) != game_is_wrapping(g2)) return false;
    if (game_is_diagadj(g1) != game_is_diagadj(g2)) return false;

    return true;
}

void game_delete(game g) {
    assert(g);

    free(g->squares);
    free(g->nb_tents_row);
    free(g->nb_tents_col);
    free(g->settings);
    free(g->action->undo->content);
    free(g->action->redo->content);
    free(g->action->undo);
    free(g->action->redo);
    free(g->action);
    free(g);
}

void game_set_square(game g, uint i, uint j, square s) {
    assert(g && i < game_nb_rows(g) && j < game_nb_cols(g));
    assert(s == EMPTY || s == GRASS || s == TENT || s == TREE);
    g->squares[get_array_index(g, i, j)] = s;
}

square game_get_square(cgame g, uint i, uint j) {
    assert(g && i < game_nb_rows(g) && j < game_nb_cols(g));
    return g->squares[get_array_index(g, i, j)];
}

void game_set_expected_nb_tents_row(game g, uint i, uint nb_tents) {
    assert(g && i < game_nb_rows(g));
    g->nb_tents_row[i] = nb_tents;
}

void game_set_expected_nb_tents_col(game g, uint j, uint nb_tents) {
    assert(g && j < game_nb_cols(g));
    g->nb_tents_col[j] = nb_tents;
}

uint game_get_expected_nb_tents_row(cgame g, uint i) {
    assert(g && i < game_nb_rows(g));
    return g->nb_tents_row[i];
}

uint game_get_expected_nb_tents_col(cgame g, uint j) {
    assert(g && j < game_nb_cols(g));
    return g->nb_tents_col[j];
}

uint game_get_expected_nb_tents_all(cgame g) {
    assert(g);
    return nb_square_all(g, TREE);
}

uint game_get_current_nb_tents_row(cgame g, uint i) {
    assert(g && i < game_nb_rows(g));
    return nb_square_row(g, i, TENT);
}

uint game_get_current_nb_tents_col(cgame g, uint j) {
    assert(g && j < game_nb_cols(g));
    return nb_square_col(g, j, TENT);
}

uint game_get_current_nb_tents_all(cgame g) {
    assert(g);
    return nb_square_all(g, TENT);
}

void game_play_move(game g, uint i, uint j, square s) {
    assert(g && i < game_nb_rows(g) && j < game_nb_cols(g));
    assert(s == EMPTY || s == GRASS || s == TENT);

    // Put element on undo stack (for history)
    move new_move = make_move(g, i, j, s);
    stack_push(g->action->undo, new_move);
    stack_dump(g->action->redo);  // If we play a move, we have to empty redo history

    // All moves are authorized EXCEPT ILLEGAL ones (already check). If it's not, placing a square at (i,j) pos.
    game_set_square(g, i, j, s);
}

bool game_is_over(cgame g) {
    assert(g);
    // If all rules for winning are true, game is won !
    return check_rule1(g) && check_rule2(g) && check_rule3(g) && check_rule4(g);
}

void game_fill_grass_row(game g, uint i) {
    assert(g && i < game_nb_rows(g));
    fill_square_row(g, i, GRASS);  // Put grass on i row only when (i, j) square is empty
}

void game_fill_grass_col(game g, uint j) {
    assert(g && j < game_nb_cols(g));
    fill_square_col(g, j, GRASS);  // Put grass on j col only when (i, j) square is empty
}

void game_restart(game g) {
    assert(g);

    for (uint i = 0; i < game_nb_rows(g); i++)
        for (uint j = 0; j < game_nb_cols(g); j++)
            if (!check_square_tree(g, i, j))  // If (i, j) square is not a tree, (i, j) is reset at empty
                game_set_square(g, i, j, EMPTY);

    // Game restart imply empty history
    stack_dump(g->action->undo);
    stack_dump(g->action->redo);
}

int game_check_move(cgame g, uint i, uint j, square s) {
    assert(g && i < game_nb_rows(g) && j < game_nb_cols(g));
    assert(s == EMPTY || s == GRASS || s == TENT || s == TREE);

    if (check_square_tree(g, i, j) || s == TREE)  // Illegal move case
        return ILLEGAL;
    if (check_square_value(g, i, j, s))  // If (i, j) square is s, that is a regular move
        return REGULAR;

    uint mouv = REGULAR;
    game copy = game_copy(g);
    game_play_move(copy, i, j, s);

    if (s == TENT) {
        // Losing move (no tent adj)
        if (neigh_get_square(copy, i, j, TENT, !game_is_diagadj(copy))) {
            game_delete(copy);
            return LOSING;
        }

        if (!neigh_get_square(copy, i, j, TREE, false)) {  // Tent no get tree -> losing
            game_delete(copy);
            return LOSING;
        }

        // Losing move (current > expected)
        if (game_get_current_nb_tents_all(g) >= game_get_expected_nb_tents_all(g)) {
            game_delete(copy);
            return LOSING;
        }
        if (game_get_current_nb_tents_row(g, i) >= game_get_expected_nb_tents_row(g, i)) {
            game_delete(copy);
            return LOSING;
        }
        if (game_get_current_nb_tents_col(g, j) >= game_get_expected_nb_tents_col(g, j)) {
            game_delete(copy);
            return LOSING;
        }
    }

    if (s == GRASS) {
        // Losing move (too much grass)
        if (game_get_expected_nb_tents_row(g, i) - game_get_current_nb_tents_row(g, i) >= nb_square_row(g, i, EMPTY)) {
            game_delete(copy);
            return LOSING;
        }
        if (game_get_expected_nb_tents_col(g, j) - game_get_current_nb_tents_col(g, j) >= nb_square_col(g, j, EMPTY)) {
            game_delete(copy);
            return LOSING;
        }

        // If (i, j) is surrounded by grass -> losing
        for (uint i = 0; i < game_nb_rows(g); i++)
            for (uint j = 0; j < game_nb_cols(g); j++)
                if (check_square_tree(copy, i, j))
                    if (neigh_all_get_square(copy, i, j, GRASS)) {
                        game_delete(copy);
                        return LOSING;
                    }
    }

    game_delete(copy);  // Frees the game copy
    return mouv;
}

/* ************************************************************************** */
/*                             EXTERNAL FUNCTIONS                             */
/* ************************************************************************** */

// Allocates memory to arrays and sets parameter values of a game
static game alloc_game(uint nb_rows, uint nb_cols, bool wrapping, bool diagadj) {
    game new_game = malloc(sizeof(game_s));
    assert(new_game);

    // Allocates space for the squares tab on new_game
    new_game->squares = (square *)malloc(nb_rows * nb_cols * sizeof(square));
    new_game->nb_tents_row = (uint *)malloc(nb_rows * sizeof(uint));
    new_game->nb_tents_col = (uint *)malloc(nb_cols * sizeof(uint));
    new_game->settings = (param *)malloc(sizeof(param));
    new_game->action = malloc(sizeof(d_stack));

    assert(new_game->nb_tents_row && new_game->nb_tents_col && new_game->squares && new_game->settings && new_game->action);

    // We initialize the parameters with the desired values and create the history stacks
    new_game->settings->nb_rows = nb_rows;
    new_game->settings->nb_cols = nb_cols;
    new_game->settings->wrapping = wrapping;
    new_game->settings->diagadj = diagadj;

    new_game->action->redo = stack_create_empty();
    new_game->action->undo = stack_create_empty();

    return new_game;
}

game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping, bool diagadj) {
    game new_game = alloc_game(nb_rows, nb_cols, wrapping, diagadj);  // Alloc a new_game with expected parameters

    // Seting squares to EMPTY state, and initializing expected number of tents per col & row to none
    for (uint i = 0; i < nb_rows; i++) {
        for (uint j = 0; j < nb_cols; j++) {
            if (i == 0)
                game_set_expected_nb_tents_col(new_game, j, 0);  // Initialised expected nb tents col at 0
            game_set_square(new_game, i, j, EMPTY);  // Initialised all squares at empty
        }
        game_set_expected_nb_tents_row(new_game, i, 0);  // Initialised expected nb tents row at 0
    }
    return new_game;
}

game game_new_ext(uint nb_rows, uint nb_cols, square *squares, uint *nb_tents_row, uint *nb_tents_col, bool wrapping, bool diagadj) {
    valid_input(squares, nb_tents_row, nb_tents_col, nb_rows, nb_cols);  // Check all input : if problem -> exit

    game new_game = game_new_empty_ext(nb_rows, nb_cols, wrapping, diagadj);

    // Setting square values and initializing expected number of tents per col & row
    for (uint i = 0; i < nb_rows; i++) {
        for (uint j = 0; j < nb_cols; j++) {
            if (i == 0)
                game_set_expected_nb_tents_col(new_game, j, nb_tents_col[j]);  // Initialised expected nb tents col at expected value
            game_set_square(new_game, i, j, squares[get_array_index(new_game, i, j)]);  // Initialised all squares at expected value
        }
        game_set_expected_nb_tents_row(new_game, i, nb_tents_row[i]);  // Initialised expected nb tents row at expected value
    }
    return new_game;
}

uint game_nb_rows(cgame g) {
    assert(g);
    return g->settings->nb_rows;
}
uint game_nb_cols(cgame g) {
    assert(g);
    return g->settings->nb_cols;
}
bool game_is_wrapping(cgame g) {
    assert(g);
    return g->settings->wrapping;
}
bool game_is_diagadj(cgame g) {
    assert(g);
    return g->settings->diagadj;
}

void game_undo(game g) {
    assert(g);

    // Undo only works if a play has been done previously
    if (!stack_is_empty(g->action->undo)) {
        move last_move = stack_top_value(g->action->undo);  // We get the last value played
        move new_move = make_move(g, last_move.x, last_move.y, last_move.new);  // We create a movement with the recovered value

        // Pushing the undone move into the redo stack, pops the undo stack and replays the previous action.
        stack_push(g->action->redo, new_move);
        stack_pop(g->action->undo);
        game_set_square(g, last_move.x, last_move.y, last_move.old);
    }
}

void game_redo(game g) {
    assert(g);

    // Redo only works if a play has been undone previously
    if (!stack_is_empty(g->action->redo)) {
        move last_move = stack_top_value(g->action->redo);  // We get the last value unplayed
        move new_move = make_move(g, last_move.x, last_move.y, last_move.new);  // We create a movement with the recovered value

        // Pushing the redone move into the undo stack, pops the redo stack and replays the previous action.
        stack_push(g->action->undo, new_move);
        stack_pop(g->action->redo);
        game_set_square(g, last_move.x, last_move.y, last_move.old);
    }
}