#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "header/game.h"
#include "header/game_aux.h"
#include "header/game_ext.h"
#include "header/game_tools.h"

void usage(char *nom) {
    fprintf(stderr, "Usage: %s <option> <input> [<output>]\n", nom);
    fprintf(stderr, "-s -> solve game / -c -> count solutions \n");
    fprintf(stderr, "output file path not mandatory.\n");
}

// Checks if the input file exists
bool file_exists(char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int main(int argc, char *argv[]) {
    // Cas d'erreur sur le nombre d'arguments
    if (argc > 4 || argc < 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Cas d'erreur sur l'option
    if (!(strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "-c") == 0)) {
        fprintf(stderr, "<option> (%s) is not valid.\n", argv[1]);
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Vérification de l'existence du fichier <input>
    if (!file_exists(argv[2])) {
        fprintf(stderr, "<input> file (%s) does not exist.\n", argv[2]);
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Traitement en fonction des deux options
    game c_game = game_load(argv[2]);

    if (strcmp(argv[1], "-s") == 0) {
        printf("Game loaded from command line : %s\n", argv[2]);
        bool is_game_solved = game_solve(c_game);
        if (is_game_solved) {
            if (argc == 4) {
                game_save(c_game, argv[3]);
                printf("Saved in %s", argv[3]);
            } else
                game_print(c_game);
            game_delete(c_game);
            return EXIT_SUCCESS;
        }
        printf("This game can't be solved.\n");
        game_delete(c_game);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-c") == 0) {
        uint solutions = game_nb_solutions(c_game);
        if (argc == 4) {
            assert(argv[3]);
            FILE *save = fopen(argv[3], "w");
            fprintf(save, "%u\n", solutions);
            fclose(save);
            printf("Saved in %s", argv[3]);
        } else
            printf("There is/are %u solution(s) to this game.\n", solutions);
        game_delete(c_game);
        return EXIT_SUCCESS;
    }
    game_delete(c_game);
    return EXIT_SUCCESS;
}
