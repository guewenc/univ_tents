#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "header/game.h"
#include "header/game_aux.h"
#include "header/game_ext.h"
#include "header/game_tools.h"

// Prints a series of infos on how to play
void help() {
    printf("> action: help\n");
    printf("- press 't <i> <j>' to put a tent at square (i,j)\n");
    printf("- press 'g <i> <j>' to put a grass at square (i,j)\n");
    printf("- press 'e <i> <j>' to erase square (i,j)\n");
    printf("- press 'z' to undo\n");
    printf("- press 'y' to redo\n");
    printf("- press 'r' to restart\n");
    printf("- press 's' to save\n");
    printf("- press 'q' to quit\n\n");
}

// Checks that a move is legal, then plays it
void play(game c_game, uint x, uint y, square square, char c) {
    if (game_check_move(c_game, x, y, square) != ILLEGAL) {
        game_play_move(c_game, x, y, square);
        printf("> action: play move '%c' into square (%u, %u)\n", c, x, y);
    }
}

// Ends the program and frees the game, if lost
void quit_game_loss(game c_game) {
    printf("> action: quit\n");
    printf("What a shame, you gave up :-(\n");
    game_delete(c_game);
    exit(EXIT_SUCCESS);
}

// Ends the program and frees the game, if won
void quit_game_won(game c_game) {
    printf("congratulations !\n");
    game_delete(c_game);
}

void save(game c_game) {
    printf("> insert a path to a destination file");
    char path[50];
    scanf("%s50", path);
    game_save(c_game, path);
}

void usage(char *nom) {
    fprintf(stderr, "Usage: %s path of game\n", nom);
    fprintf(stderr, "Example: %s ../data/game_default.tnt\n", nom);
}

// Checks if the input file exists
bool file_exists(char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    game c_game = NULL;

    if (argc == 2) {
        if (!file_exists(argv[1])) {
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
        printf("Game loaded from command line : %s\n", argv[1]);
        c_game = game_load(argv[1]);
    } else
        c_game = game_default();

    game_print(c_game);

    char c;
    uint x, y;

    while (!game_is_over(c_game)) {
        printf("> ? [h for help]\n");
        int check = scanf("%c", &c);
        int check2 = 0;

        if (c != 'h' && c != 'r' && c != 'q' && c != 'z' && c != 'y' && c != 's') {
            check2 = scanf("%d %d", &x, &y);
        }

        if (check == EOF)
            quit_game_loss(c_game);

        switch (check) {
            case 1:
                switch (c) {
                    case 'h':
                        help();
                        break;
                    case 'z':
                        game_undo(c_game);
                        break;
                    case 'y':
                        game_redo(c_game);
                        break;
                    case 'r':
                        game_restart(c_game);
                        printf("> action: restart\n");
                        break;
                    case 's':
                        save(c_game);
                        break;
                    case 'q':
                        if (check == 1)
                            quit_game_loss(c_game);
                        break;
                }

            case 3:
                if ((check + check2) == 3) {
                    if (c == 't')
                        play(c_game, x, y, TENT, c);
                    else if (c == 'g')
                        play(c_game, x, y, GRASS, c);
                    else if (c == 'e')
                        play(c_game, x, y, EMPTY, c);
                }
        }
        game_print(c_game);
    }
    quit_game_won(c_game);
    return EXIT_SUCCESS;
}
