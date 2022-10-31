#include "header/game_tools.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "header/game.h"
#include "header/game_ext.h"
#include "header/private.h"

typedef struct game_and_nb {
    game g;
    uint nb;
} game_and_nb;

/* ************************************************************************** */
/*                           SAVE / LOAD FUNCTIONS                            */
/* ************************************************************************** */

game game_load(char *filename) {
    assert(filename);

    // Step 0: Open the file and create the game
    FILE *load_file = fopen(filename, "r");  // Opening the file
    game new_game = NULL;  // Game declaration: NULL if invalid given path

    if (load_file) {
        // Step 1: retrieve the parameters (first line)
        uint nb_rows = 0;
        uint nb_cols = 0;
        int wrapping = 0;
        int diagadj = 0;

        fscanf(load_file, "%u %u %d %d\n", &nb_rows, &nb_cols, &wrapping, &diagadj);

        uint *nb_tents_row = (uint *)calloc(nb_rows, sizeof(uint));
        uint *nb_tents_col = (uint *)calloc(nb_cols, sizeof(uint));

        for (uint i = 0; i < nb_rows; i++)
            fscanf(load_file, "%u", &nb_tents_row[i]);

        for (uint j = 0; j < nb_cols; j++)
            fscanf(load_file, "%u", &nb_tents_col[j]);
        fgetc(load_file);  // To skip the line feed
        fgetc(load_file);  // To skip the line feed

        // Step 3: Collect all the squares
        square *squares = malloc(nb_rows * nb_cols * sizeof(square));

        for (uint i = 0; i < nb_rows; i++) {
            for (uint j = 0; j < nb_cols; j++)
                squares[get_array_index_aux(i, j, nb_cols)] = char_to_square(fgetc(load_file));
            fgetc(load_file);  // To skip the line feed
        }

        // Step 4: Creation of the game
        new_game = game_new_ext(nb_rows, nb_cols, squares, nb_tents_row, nb_tents_col, wrapping, diagadj);

        // Step 5: Freeing the allocated variables
        free(nb_tents_row);
        free(nb_tents_col);
        free(squares);

        fclose(load_file);  // Close the file
    } else
        printf("The file could not be opened. Is this the right way?");

    return new_game;
}

void game_save(cgame g, char *filename) {
    assert(filename);
    FILE *save = fopen(filename, "w");  // w = if filename already exists, it completely overwrites it

    // Line 1: game parameters
    fprintf(save, "%d %d %d %d\n", game_nb_rows(g), game_nb_cols(g), game_is_wrapping(g), game_is_diagadj(g));

    // Line 2 : nb_tents_row
    for (uint i = 0; i < game_nb_rows(g); i++)
        fprintf(save, "%d ", game_get_expected_nb_tents_row(g, i));
    fprintf(save, "\n");

    // Line 3 : nb_tents_col
    for (uint j = 0; j < game_nb_cols(g); j++)
        fprintf(save, "%d ", game_get_expected_nb_tents_col(g, j));
    fprintf(save, "\n");

    // Lines 4 -> nb_rows
    for (uint row = 0; row < game_nb_rows(g); row++) {
        for (uint col = 0; col < game_nb_cols(g) - 1; col++)
            fprintf(save, "%c", square_to_char(game_get_square(g, row, col)));
        fprintf(save, "%c\n", square_to_char(game_get_square(g, row, game_nb_cols(g) - 1)));
    }

    fclose(save);
}

/* ************************************************************************** */
/*                              SOLVER FUNCTIONS                              */
/* ************************************************************************** */

// Create an empty game_and_nb struct
static game_and_nb empty_game_nb() {
    game_and_nb tmp;
    tmp.g = NULL;
    tmp.nb = 0;
    return tmp;
}

/* ********************* COMBINATORIAL / SEQUENCES PART ********************* */

// Get the first sequence of an (n, k) combinatorial class
static int *firstSequence(int n, int k) {
    int *s = calloc(k, sizeof(int));
    assert(s);
    for (int i = 0; i < k; i++)  // (1, 2, ..., k - 1)
        s[i] = i;
    return s;
}

// Get the last sequence of an (n, k) combinatorial class
static int *lastSequence(int n, int k) {
    int *l = calloc(k, sizeof(int));
    assert(l);
    for (int i = 0; i < k; i++)  // (n-1, n-2, ..., n-k+1)
        l[k - i - 1] = n - i - 1;
    return l;
}

// Get the next sequence of an (n, k) combinatorial class compared to s
static int *nextOfSequence(int n, int k, int *s) {
    assert(s);
    int cpt = 0;
    for (int i = 0; i < k; i++) {
        if (s[k - i - 1] < (n - 1 - i)) {
            s[k - i - 1] += 1;
            cpt = k - i;
            break;
        }
    }
    for (int i = cpt; i < k; i++)
        s[i] = s[i - 1] + 1;
    return s;
}

// Return true if s1 and s2 sequences are equal; false otherwise
static bool sequencesAreEqual(int *s1, int *s2, int s) {
    assert(s1 && s2);
    for (uint i = 0; i < s; i++)
        if (s1[i] != s2[i])
            return false;
    return true;
}

static void incrementAtPos(int *s, int *l, uint pos, uint n, uint k) {
    assert(s && l);
    if (pos == 0)
        return;
    if (s[pos] == l[pos]) {
        incrementAtPos(s, l, pos - 1, n, k);
        return;
    }
    s[pos]++;
    for (int i = pos + 1; i < k; i++) {
        s[i] = s[i - 1] + 1;
    }
}

/* ************************************************************************** */

static bool optimise_game_is_over(cgame g) {
    uint cpt_tent = 0;
    uint cpt_tree = 0;

    uint cpt_tent_row = 0;

    for (uint i = 0; i < game_nb_rows(g); i++) {
        for (uint j = 0; j < game_nb_cols(g); j++) {
            if (i == 0)
                if (nb_square_col(g, j, TENT) != game_get_expected_nb_tents_col(g, j))
                    return false;

            // Rule 1
            if (check_square_tent(g, i, j))
                if (neigh_get_square(g, i, j, TENT, !game_is_diagadj(g)))
                    return false;

            // Rule 3
            if (game_get_square(g, i, j) == TENT) {
                cpt_tent++;
                cpt_tent_row++;
            }
            if (game_get_square(g, i, j) == TREE) cpt_tree++;

            // Rule 4
            if (check_square_tree(g, i, j))  // (i, j) tree get a tent on his orthogonally neighbors.
                if (!neigh_get_square(g, i, j, TENT, false))
                    return false;
            if (check_square_tent(g, i, j))  // (i, j) tent get a tree on his orthogonally neighbors.
                if (!neigh_get_square(g, i, j, TREE, false))
                    return false;
        }

        if (cpt_tent_row != game_get_expected_nb_tents_row(g, i))
            return false;
        cpt_tent_row = 0;
    }
    return cpt_tent == cpt_tree;
}

static bool isImpossibleSequence(cgame g, coor *a, int *s, int *l, uint n, uint k) {
    assert(s);
    uint row = 0;
    uint *count_row = calloc(game_nb_rows(g), sizeof(uint));
    assert(count_row);

    for (uint i = 0; i < k; i++) {
        row = row_of_coor(a[s[i]]);
        count_row[row]++;

        if (count_row[row] > game_get_expected_nb_tents_row(g, row)) {
            incrementAtPos(s, l, i, n, k);
            free(count_row);
            return false;
        }

        if (i != k - 1) {
            if (row_of_coor(a[s[i]]) == row_of_coor(a[s[i + 1]])) {
                if (col_of_coor(a[s[i]]) + 1 == col_of_coor(a[s[i + 1]])) {
                    incrementAtPos(s, l, i + 1, n, k);
                    free(count_row);
                    return false;
                }
            }
        }
    }
    free(count_row);
    return false;
}

// Return one solution of the g game
static game getOneSolution(coor *a, int n, int k, cgame g) {
    int *s = firstSequence(n, k);
    int *l = lastSequence(n, k);
    game copy_game = game_copy(g);

    while (s != NULL) {  // As long as there is a sequence to use
        for (uint i = 0; i < k; i++)  // Copy all tents of the sequences
            game_play_move(copy_game, row_of_coor(a[s[i]]), col_of_coor(a[s[i]]), TENT);

        if (optimise_game_is_over(copy_game)) {  // If we've got a winning game
            free(s);
            free(l);
            return copy_game;
        }

        if (sequencesAreEqual(s, l, k)) {  // If is the end of program -> we can't find solutions
            free(s);
            s = NULL;
        } else {
            do {
                s = nextOfSequence(n, k, s);
            } while (isImpossibleSequence(copy_game, a, s, l, n, k));
        }

        for (uint i = 0; i < k; i++)
            game_undo(copy_game);
    }
    game_delete(copy_game);
    free(s);
    free(l);
    return NULL;  // Return an empty game because there are no solutions
}

// Return the number of solutions of the g game
static uint getNbrSolution(coor *a, int n, int k, cgame g) {
    int *s = firstSequence(n, k);
    int *l = lastSequence(n, k);
    game copy_game = game_copy(g);
    uint cpt = 0;

    while (s != NULL) {  // As long as there is a sequence to use
        for (uint i = 0; i < k; i++)  // Copy all tents of the sequences
            game_play_move(copy_game, row_of_coor(a[s[i]]), col_of_coor(a[s[i]]), TENT);

        if (optimise_game_is_over(copy_game))  // If we've got a winning game
            cpt++;

        if (sequencesAreEqual(s, l, k)) {  // If is the end of program -> we can't find solutions
            free(s);
            s = NULL;
        } else {
            do {
                s = nextOfSequence(n, k, s);
            } while (isImpossibleSequence(copy_game, a, s, l, n, k));
        }

        for (uint i = 0; i < k; i++)
            game_undo(copy_game);
    }
    game_delete(copy_game);
    free(s);
    free(l);
    return cpt;
}

/* ************************************************************************** */

// Function linked to valid_tents_with_one_tree: validate a tent on a tree
static void valid_unique_tent(game gc, game gs, uint i, uint j) {
    if (neigh_check_square(gc, i, j, NORTH, TENT)) {
        neigh_set_square(gs, i, j, NORTH, TENT);
        neigh_set_square(gc, i, j, NORTH, EMPTY);
        return;
    }

    if (neigh_check_square(gc, i, j, SOUTH, TENT)) {
        neigh_set_square(gs, i, j, SOUTH, TENT);
        neigh_set_square(gc, i, j, SOUTH, EMPTY);
        return;
    }

    if (neigh_check_square(gc, i, j, WEST, TENT)) {
        neigh_set_square(gs, i, j, WEST, TENT);
        neigh_set_square(gc, i, j, WEST, EMPTY);
        return;
    }

    if (neigh_check_square(gc, i, j, EAST, TENT)) {
        neigh_set_square(gs, i, j, EAST, TENT);
        neigh_set_square(gc, i, j, EAST, EMPTY);
        return;
    }
}

// Place tents everywhere except where there is expected to be 0
static void place_tents_everywhere(game g) {
    for (uint i = 0; i < game_nb_rows(g); i++)
        for (uint j = 0; j < game_nb_cols(g); j++)
            if (game_get_expected_nb_tents_row(g, i) != 0 && game_get_expected_nb_tents_col(g, j) != 0)
                if (!check_square_tree(g, i, j))
                    game_set_square(g, i, j, TENT);
}

// Remove unnecessary tents
static void remove_unnecessary_tents(game g) {
    for (uint i = 0; i < game_nb_rows(g); i++)
        for (uint j = 0; j < game_nb_cols(g); j++)
            if (neigh_count(g, i, j, TREE, false) == 0 && !check_square_tree(g, i, j))
                game_set_square(g, i, j, EMPTY);
}

// Copy of tents necessarily well placed in the game gc
static void copy_safe_tents(game gc, game gs) {
    for (uint j = 0; j < game_nb_cols(gc); j++)
        if (game_get_current_nb_tents_col(gc, j) + game_get_current_nb_tents_col(gs, j) == game_get_expected_nb_tents_col(gs, j))
            for (uint i = 0; i < game_nb_rows(gs); i++)
                if (check_square_tent(gc, i, j))
                    game_set_square(gs, i, j, TENT);
}

// Delete all tents necessarily well placed in the game gs (with nb_expected)
static void delete_tents_expected(game gc, game gs) {
    for (uint i = 0; i < game_nb_rows(gs); i++)
        if (game_get_expected_nb_tents_row(gs, i) == game_get_current_nb_tents_row(gs, i)) {
            for (uint j = 0; j < game_nb_cols(gs); j++)
                if (!check_square_tree(gc, i, j))
                    game_set_square(gc, i, j, EMPTY);
        }
}

// If play tent on (i, j) position on gs game is LOSING, we can delete this tent
static void remove_with_losing(game gc, cgame gs) {
    for (uint i = 0; i < game_nb_rows(gc); i++)
        for (uint j = 0; j < game_nb_cols(gc); j++)
            if (check_square_tent(gc, i, j))
                if (game_check_move(gs, i, j, TENT) == LOSING)
                    game_set_square(gc, i, j, EMPTY);
}

// If a tree get only one tent : we can valid it
static void valid_tents_with_one_tree(game gc, game gs) {
    for (uint i = 0; i < game_nb_rows(gc); i++)
        for (uint j = 0; j < game_nb_cols(gc); j++)
            if (check_square_tree(gc, i, j))
                if (neigh_count(gc, i, j, TENT, false) == 1 && neigh_count(gs, i, j, TENT, false) == 0) {
                    valid_unique_tent(gc, gs, i, j);
                    game_set_square(gc, i, j, EMPTY);
                }
}

// Place tents all over the game
static game place_all_tents(cgame g) {
    game gc = game_copy(g);
    place_tents_everywhere(gc);
    remove_unnecessary_tents(gc);
    return gc;
}

// Only keep tents on a game
static void only_keep_tents(game gc, game gs) {
    for (uint i = 0; i < game_nb_rows(gc); i++)
        for (uint j = 0; j < game_nb_cols(gc); j++)
            if (game_get_square(gc, i, j) == game_get_square(gs, i, j))
                game_set_square(gc, i, j, EMPTY);
}

/* ************************************************************************** */

/* This function is common to both functions solve and nb_solutions
 * The logic adopted in order to reduce the number of tent solutions to be placed as much as possible is as follows:
 *      Step 1 : We start by placing tents everywhere on the game (except on the trees).
 *      Step 2 : Then, we can delete all the tents that are on a row or column where the number of expected tents is 0.
 *
 *      Step 3 : We copy all the tents that are already present on the game in question.
 *               If one of the placements is a LOSING placement, the game will inevitably have no solutions so we stop the algorithm.
 *      Step 4 : A series of functions are applied which aim to considerably reduce the number of combinations to be tested.
 *                  -> First : All the tents that satisfy the "expected" rule are placed.
 *                  -> Second : All tents that cause LOSING movement are deleted.
 *                  -> Third : All the tents linked to a single tree which is itself alone are placed.
 *                  -> Fourth : All tents that cannot be placed (expected rule) are deleted.
 *
 *      Step 5 : We create a table with all the potential tents that remain to be placed. We then test all possible tent combinations.
*/
static game_and_nb common_treatment(game g, bool solve, bool count) {
    assert(g);
    game_and_nb rt = empty_game_nb();

    game gc = place_all_tents(g);  // Step 1 / 2

    uint nb_sol = 0;
    game gs = game_copy(gc);
    game_restart(gs);

    // Step 3
    for (uint i = 0; i < game_nb_rows(g); i++)
        for (uint j = 0; j < game_nb_cols(g); j++)
            if (check_square_tent(g, i, j)) {
                if (game_check_move(gs, i, j, TENT) == LOSING)
                    return rt;
                else
                    game_set_square(gs, i, j, TENT);
            }

    bool changement = true;
    game check_changement = NULL;

    // Step 4
    while (changement) {
        changement = false;
        check_changement = game_copy(gs);

        copy_safe_tents(gc, gs);  // First
        remove_with_losing(gc, gs);  // Second
        valid_tents_with_one_tree(gc, gs);  // Third
        delete_tents_expected(gc, gs);  // Fourth

        if (!game_equal(gs, check_changement))
            changement = true;
        game_delete(check_changement);
    }

    if (nb_square_all(gc, TENT) < nb_square_all(gs, TREE) - nb_square_all(gs, TENT))
        return rt;

    // Step 5
    only_keep_tents(gc, gs);

    coor *remain_coor = calloc(nb_square_all(gc, TENT) + 1, sizeof(coor));
    assert(remain_coor);
    uint index = 0;

    for (int i = 0; i < game_nb_rows(gc); i++)
        for (int j = 0; j < game_nb_cols(gc); j++)
            if (check_square_tent(gc, i, j)) {
                remain_coor[index] = make_coor(i, j);
                index++;
            }

    int n = nb_square_all(gc, TENT);
    int k = nb_square_all(gs, TREE) - nb_square_all(gs, TENT);
    nb_sol = 1;

    game solution = NULL;

    if (k > 0) {  // Game is already finished
        if (solve) {
            solution = getOneSolution(remain_coor, n, k, gs);
            if (!gs)
                return rt;
        }

        if (count)
            nb_sol = getNbrSolution(remain_coor, n, k, gs);
    } else {
        if (solve)
            solution = game_copy(gs);
    }

    if (solve)
        rt.g = solution;
    rt.nb = nb_sol;

    game_delete(gc);
    game_delete(gs);
    free(remain_coor);
    return rt;
}

bool game_solve(game g) {
    game_and_nb rt = common_treatment(g, true, false);

    if (rt.g == NULL)
        return false;

    // Copy of all winning game
    for (int i = 0; i < game_nb_rows(rt.g); i++)
        for (int j = 0; j < game_nb_cols(rt.g); j++)
            game_set_square(g, i, j, game_get_square(rt.g, i, j));
    game_delete(rt.g);
    return true;
}

uint game_nb_solutions(game g) {
    game_and_nb rt = common_treatment(g, false, true);
    return rt.nb;
}