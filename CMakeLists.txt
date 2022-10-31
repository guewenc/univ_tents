cmake_minimum_required(VERSION 2.8)
project(game_text C)

enable_testing()
include(CTest)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS "-std=c99 -Wall")
set(CMAKE_C_FLAGS_DEBUG "-g --coverage")
set(CMAKE_C_FLAGS_RELEASE "-Ofast")

add_library(game game.c game_aux.c game_tools.c private.c)

add_executable(game_text game_text.c)
target_link_libraries(game_text game)

add_executable(game_test game_test.c)
target_link_libraries(game_test game)

add_executable(game_solve game_solve.c)
target_link_libraries(game_solve game)

# ------ MATTEO'S TESTS ------ #
add_test(test_mdavoigneau_copy ./game_test copy)
add_test(test_mdavoigneau_delete ./game_test delete)
add_test(test_mdavoigneau_fill_grass_col ./game_test fill_grass_col)
add_test(test_mdavoigneau_fill_grass_row ./game_test fill_grass_row)
add_test(test_mdavoigneau_default ./game_test game_default)
add_test(test_mdavoigneau_default_solution ./game_test game_default_solution)
add_test(test_mdavoigneau_check_move_0 ./game_test check_move_0) # Illegal
add_test(test_mdavoigneau_check_move_1 ./game_test check_move_1) # Losing tent with diagadj
add_test(test_mdavoigneau_check_move_2 ./game_test check_move_2) # Losing tent without diagadj
add_test(test_mdavoigneau_check_move_3 ./game_test check_move_3) # Losing tent not get tree
add_test(test_mdavoigneau_check_move_4 ./game_test check_move_4) # Losing tent current >= expected
add_test(test_mdavoigneau_check_move_5 ./game_test check_move_5) # Losing grass empty tent row
add_test(test_mdavoigneau_check_move_6 ./game_test check_move_6) # Losing grass empty tent col
add_test(test_mdavoigneau_check_move_7 ./game_test check_move_7) # Regular
add_test(test_mdavoigneau_check_move_8 ./game_test check_move_8) # Losing tent with wrapping
add_test(test_mdavoigneau_equal_0 ./game_test equal_0) # Check game is equal
add_test(test_mdavoigneau_equal_1 ./game_test equal_1) # Check settings

# ------ MARION'S TESTS ------ #
add_test(test_mfaidy_get_current_nb_tents_all ./game_test get_current_nb_tents_all)
add_test(test_mfaidy_get_current_nb_tents_col ./game_test get_current_nb_tents_col)
add_test(test_mfaidy_get_current_nb_tents_row ./game_test get_current_nb_tents_row)
add_test(test_mfaidy_get_expected_nb_tents_all ./game_test get_expected_nb_tents_all)
add_test(test_mfaidy_get_expected_nb_tents_col ./game_test get_expected_nb_tents_col)
add_test(test_mfaidy_get_expected_nb_tents_row ./game_test get_expected_nb_tents_row)
add_test(test_mfaidy_get_square ./game_test get_square)
add_test(test_mfaidy_game_print ./game_test game_print)
add_test(test_mfaidy_nb_rows ./game_test nb_rows)
add_test(test_mfaidy_nb_cols ./game_test nb_cols)
add_test(test_mfaidy_new_empty_ext ./game_test new_empty_ext)
add_test(test_mfaidy_new_ext ./game_test new_ext)
add_test(test_mfaidy_is_wrapping ./game_test is_wrapping)
add_test(test_mfaidy_is_diagadj ./game_test is_diagadj)
add_test(test_mfaidy_undo ./game_test undo)
add_test(test_mfaidy_redo ./game_test redo)
add_test(test_mfaidy_solve ./game_test solve)
add_test(test_mfaidy_nb_solutions ./game_test nb_sol)

# ------ GUEWEN'S TESTS ------ #
add_test(test_gcousseau_new ./game_test new)
add_test(test_gcousseau_new_empty ./game_test new_empty)
add_test(test_gcousseau_play_move ./game_test play_move)
add_test(test_gcousseau_restart ./game_test restart)
add_test(test_gcousseau_set_expected_nb_tents_col ./game_test set_expected_nb_tents_col)
add_test(test_gcousseau_set_expected_nb_tents_row ./game_test set_expected_nb_tents_row)
add_test(test_gcousseau_set_square ./game_test set_square)
add_test(test_gcousseau_is_over_0 ./game_test is_over_0) # Check losing game
add_test(test_gcousseau_is_over_1 ./game_test is_over_1) # Check rule 1
add_test(test_gcousseau_is_over_2 ./game_test is_over_2) # Check rule 1 diagadj
add_test(test_gcousseau_is_over_3 ./game_test is_over_3) # Check rule 1 wrapping
add_test(test_gcousseau_is_over_4 ./game_test is_over_4) # Check rule 1 diagadj && wrapping
add_test(test_gcousseau_is_over_5 ./game_test is_over_5) # Check rule 2
add_test(test_gcousseau_is_over_6 ./game_test is_over_6) # Check rule 3
add_test(test_gcousseau_is_over_7 ./game_test is_over_7) # Check rule 4
add_test(test_gcousseau_is_over_8 ./game_test is_over_8) # Check all -> winning game (empty && grass)
add_test(test_gcousseau_load ./game_test load)
add_test(test_gcousseau_save ./game_test save)
