#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "idklib.h"

/* TODO
 * vyřešit, když uživatel zadá --output a -o, aby program vzal jen tu druhou cestu
 * sanity checky
 * checknout, jestli input je .lp
 * check syntaxe
 * v .lp nemusí být mezery
 *
 */

#define MAX_PATH_LENGTH 255

/* Define long options */
struct option long_options[] = {
    {"output", required_argument, 0, 'O'}, /* Corresponds to --output */
    {0, 0, 0, 0} /* End of options */
};

/* ____________________________________________________________________________

    void header()

    Prints a programme header to the console - namely the programme name
    and a copyright and version information.
   ____________________________________________________________________________
*/
void header() {
    printf("\nA tool for solving linear programming problems\n");
    printf("Seminar work of \"Programming in C\"\n");
    printf("Copyright (c) Eduard Schödl, 2024\n\n");
}

/* ____________________________________________________________________________

    void help()

    Prints a short help. Shows how to use the programme, what are the
    command-line options, etc.
   ____________________________________________________________________________
*/
void help() {
    printf("\nUsage:\n");
    printf("   lp.exe <input-file> [OPTIONS]\n\n");

    printf("Options:\n");
    printf("   -o <path>, --output <path>  Specify the output file path for the solution.\n\n");

    printf("Examples:\n");
    printf("   lp.exe input.lp -o C:/example/path/to/file.lp\n");
    printf("   lp.exe input.lp --output C:/example/path/to/file.lp\n\n");

    printf("Note:\n");
    printf("   - Use either -o or --output to specify the output file path.\n\n");
}

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

char *get_input_file(int argc, char **argv) {
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

int main(const int argc, char** argv) {
    FILE *output_file_ptr = NULL, *input_file_ptr = NULL;
    char *output_path, *input_path;

	if (argc < 2) {
        header();
        help();
        return 3;
    }

    output_path = get_output_file(argc, argv);
    if (check_filename_ext(output_path, OUTPUT_FILE_EXT)) {
        printf("Invalid output file extension!\n");
        return 91;
    }

    if(output_path) {
        output_file_ptr = open_output_file(output_path);
        if (!output_file_ptr) {
            return 2;
        }
    }

    input_path = get_input_file(argc, argv);
    if (check_filename_ext(input_path, LP_EXT)) {
        printf("Invalid input file extension!\n");
        return 91;
    }

    input_file_ptr = open_input_file(input_path);
    if (!input_file_ptr) {
        return 1;
    }

    /* logika aplikace */
    read_input_file(input_file_ptr);
    solve_linear_programming();
    print_solution();

    /* If no output file was specified, print "obrazovka" */
    if (!output_path) {
        printf("obrazovka\n");
    }

    free(input_path);
    fclose(output_file_ptr);
    fclose(input_file_ptr);

    return EXIT_SUCCESS;
}
