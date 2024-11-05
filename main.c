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

int get_filename_ext(char *filename) {
    char *dot;

    dot = strrchr(filename, '.');
    if(!dot || strcmp(dot, LP_EXT) != 0) return 1;
    return 0;
}

int main(int argc, char** argv) {
	int opt;
    int option_index = 0;
    int output_specified = 0;
	FILE *output_file_ptr = NULL;
	FILE *input_file_ptr;
	char inputFile[MAX_PATH_LENGTH] = "";

	if (argc < 2) {
        header();
        help();
        return 3;
    }

    while ((opt = getopt_long(argc, argv, "o:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'o': /* Short option -o */
            case 'O': /* Long option --output */
                printf("Output path: %s\n", optarg);
                output_file_ptr = fopen(optarg, "w");

                if(!output_file_ptr) {
  					printf("Invalid output destination!\n");
  					return 2;
				}

				output_specified = 1; /* Mark that an output file was specified */
                break;
            default:
               	printf("Error unknown option\n");
        }
    }

    /* After options, remaining argument should be the input file */
    if (optind < argc) {
        strncpy(inputFile, argv[optind], MAX_PATH_LENGTH - 1);
        inputFile[MAX_PATH_LENGTH - 1] = '\0';

        if(get_filename_ext(inputFile)) {
            printf("Wrong input file extension!");
            return 10;
        }

        /* Verify the input file is valid */
        input_file_ptr = fopen(inputFile, "r");
        if (!input_file_ptr) {
            printf("Input file not found!\n");
            return 1;
		}

        printf("Input file: %s\n", inputFile);

    } else {
        printf("Error: No input file specified.\n");
        return 4;
    }

    /* logika aplikace */
    read_input_file(input_file_ptr);
    solve_linear_programming();
    print_solution();

    /* If no output file was specified, print "obrazovka" */
    if (!output_specified) {
        printf("obrazovka\n");
    }

    /* Close outputFile if it was opened */
    if (output_file_ptr) {
        fclose(output_file_ptr);
    }

    fclose(input_file_ptr);

    return EXIT_SUCCESS;
}