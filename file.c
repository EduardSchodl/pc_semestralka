#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include <io.h>
#include "file.h"

/* Define long options */
struct option long_options[] = {
    {"output", required_argument, 0, 'O'}, /* Corresponds to --output */
    {0, 0, 0, 0} /* End of options */
};

int check_filename_ext(const char *filename, const char *ext) {
    char *dot;

    dot = strrchr(filename, '.');
    if(!dot || strcmp(dot, ext) != 0) return 1;
    return 0;
}

char *get_output_file(const int argc, char **argv) {
    int opt;
    int option_index = 0;
    char *output_file_path = NULL;

    while ((opt = getopt_long(argc, argv, "o:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'o': /* Short option -o */
            case 'O': /* Long option --output */
                if(optarg[0] == '-') {
                    printf("Error: Invalid argument for option '-%c': %s!\n", opt, optarg);
                    return NULL;
                }
                output_file_path = optarg;
                break;
            default:
                break;
        }
    }

    printf("Output path: %s\n", output_file_path);
    return output_file_path;
}

char *get_input_file(const int argc, char **argv) {
    char *input_file = NULL;

    /* After options, remaining argument should be the input file */
    if (optind < argc) {
        input_file = malloc(MAX_PATH_LENGTH);

        strncpy(input_file, argv[optind], MAX_PATH_LENGTH - 1);
        input_file[MAX_PATH_LENGTH - 1] = '\0';

        printf("Input file: %s\n", input_file);
    } else {
        printf("Error: No input file specified.\n");
    }

    return input_file;
}

int file_exists(const char *file_path) {
    if (access(file_path, F_OK) == 0) {
        return 1;
    }

    return 0;
}

FILE *open_output_file(char *file_path) {
    FILE *file;

    if (!file_path) return NULL;

    file = fopen(file_path, "w");
    if (!file) {
        printf("Invalid output destination!\n");
    }
    return file;
}

FILE *open_input_file(const char *file_path) {
    FILE *file;

    if (!file_exists(file_path)) {
        printf("Input file not found!\n");
        return NULL;
    }

    file = fopen(file_path, "r");
    if (!file) {
        printf("Could not read input file!\n");
    }

    return file;
}

/* TODO
 * bude rozhazovat lines podle sekce do struct Sections
 */
char **read_input_file(FILE *input_file) {
    int lines_count = 0, i;
    char **lines;

    /* sanity check */
    if(!input_file) {
        return NULL;
    }

    /* allocate the array of lines */
    lines = malloc(MAX_LINES * sizeof(char *));
    if(!lines) {
        return NULL;
    }

    /* read all lines from the input file */
    while(lines_count < MAX_LINES && !feof(input_file)) {
        lines[lines_count] = malloc(LINE_MAX_SIZE);
        if(!lines[lines_count]) {
            for(i = 0; i < lines_count; i++) {
                free(lines[i]);
            }

            free(lines);
            lines = NULL;
            return NULL;
        }

        if(fgets(lines[lines_count], LINE_MAX_SIZE, input_file)) {
            lines_count++;
        }
    }

    return lines;
}