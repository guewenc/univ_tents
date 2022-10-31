# README `tents-a31e`

This is the private repository for Tents project (`tents-a31e`), from the *University of Bordeaux*.

## Group Members

* Guewen COUSSEAU
* Matteo DAVOIGNEAU
* Marion FAIDY

The three members are in the `INF301 A31` group.

## Auxiliary functions

### Movements system

You will find a set of movement functions in the project files. This is a quick explanation of their utility, each of them is commented whenever needed.
Here are the available movements : `NORTH`, `SOUTH`, `EAST`, `WEST` and `NORTH EAST`, `NORTH WEST`, `SOUTH EAST`, `SOUTH WEST`. This allows a better handling and understanding of the `diagadj` and `wrapping` rules functions.

To complete the movements module, a `coor` structure is used to compact the i and j square position compounds. This will again help compacting a lot of functions, thus allowing a better understanding of the project.

```c
struct coor {
    uint row, col;
} coor;
```

Movement-related functions are all located in `game.c`.

### Testing given parameters of every function

Those 4 functions are being used to help avoiding code redundancy while testing the validity of the given parameters of each function. Here is a list of the 4 functions, with a quick explanation of their utility :

* `valid_game_p(g)` stops the program if `g` pointer is not valid.
* `valid_coor_row(g, i)` stops the program if the `i` coordinate is not valid.
* `valid_coor_col(g, j)` stops the program if the `j` coordinate is not valid.
* `valid_input(squares, nb_tents_row, nb_tents_col, nb_rows, nb_cols)` stops the program if one of the given parameters is not valid.

### Factorization functions

Many functions have been coded in order to simplify the liability and avoid code duplication, thus greatly improving lisibility and error management. Here is a list of those utilitary functions :

* `display_error_and_exit` prints the encountered error and quits the program.
* `check_square_value` checks if (i,j) square is an s square.
* `check_square_empty` checks if (i,j) square is an `EMPTY` square.
* `check_square_tent` checks if (i,j) square is an `TENT` square.
* `check_square_tree` checks if (i,j) square is an `TREE` square.
* `check_pos_is_on_game` checks that (i,j) position is a valid position on the g game board.
* `get_array_index` returns the array index of the square array from a game of a given (i,j) position.
* `sum_of_array` returns the sum of the elements of a given uint array.
* `nb_square_row` counts the number of s squares in a given row.
* `nb_square_col` counts the number of s squares in a given column.
* `nb_square_all` counts the number of s squares in a given whole game board.

### Historic functions

A stack structure is used in order to store each game move. Here are the corresponding functions, quickly explained :

* `stack_create_empty` creates an empty stack.
* `stack_is_empty` tests whether a stack is empty.
* `stack_top_value` returns the value on top of the stack.
* `stack_push` adds an element on top of the stack.
* `stack_pop` erases the element on top of the stack.
* `stack_dump` erases all elements of the stack.

## Test

Here is the `game.h` and `game_aux.h` files tests repartition :

### Guewen Cousseau's tests

* `game_is_over`
* `game_new`
* `game_new_empty`
* `game_play_move`
* `game_restart`
* `game_set_expected_nb_tents_col`
* `game_set_expected_nb_tents_row`
* `game_set_square`

### Matteo Davoigneau's tests

* `game_check_move`
* `game_copy`
* `game_delete`
* `game_equal`
* `game_fill_grass_col`
* `game_fill_grass_row`
* `game_default`
* `game_default_solution`

### Marion Faidy's tests

* `game_get_current_nb_tents_all`
* `game_get_current_nb_tents_col`
* `game_get_current_nb_tents_row`
* `game_get_expected_nb_tents_all`
* `game_get_expected_nb_tents_col`
* `game_get_expected_nb_tents_row`
* `game_get_square`
* `game_print`

## `game.c` functions distribution

Here is the `game.c` file tests repartition :

### Guewen Cousseau's functions

* `game_is_over`
* `game_new`
* `game_new_empty`
* `game_play_move`
* `game_restart`
* `game_set_expected_nb_tents_col`
* `game_set_expected_nb_tents_row`
* `game_set_square`

### Matteo Davoigneau's functions

* `game_check_move`
* `game_copy`
* `game_delete`
* `game_equal`
* `game_fill_grass_col`
* `game_fill_grass_row`

### Marion Faidy's functions

* `game_get_current_nb_tents_all`
* `game_get_current_nb_tents_col`
* `game_get_current_nb_tents_row`
* `game_get_expected_nb_tents_all`
* `game_get_expected_nb_tents_col`
* `game_get_expected_nb_tents_row`
* `game_get_square`

## `game_ext` functions distribution

The `game_is_over` and `game_check_move` functions have been rewritten to comply to the new wrapping and diagadj rules.

Tests have been rewritten to comply to the new rules.
A position system (See `Movements system` in `Auxiliary functions` section) has been introduced to facilitate the reading of `squares`.
