#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "file.h"
#include "parse.h"

/* TODO
 * sanity checky
 * check syntaxe (sekce, po end nesmí nic být, povolené operátory)
 *
 * nacist cely input.lp, jednotlive radky ulozit napriklad do struktury
 * az bu doslo na END, tak by se nejdrive zpracovaly generals
 * tim by se mohlo odstranit napr boundslist, protoze by se hned mohly vazat
 * typedef struct thelp{
 *      char *generals;
 *      char **bounds;
 *      char **constraints;
 *      char *objectives;
 * } lp;
 */

/*
   ____________________________________________________________________________

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

/*
   ____________________________________________________________________________

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

int main(const int argc, char** argv) {
    int res_code;
    FILE *output_file_ptr = NULL, *input_file_ptr = NULL;
    char *output_path, *input_path;
    char **lines;

	if (argc < 2) {
        header();
        help();
        return 3;
    }

    output_path = get_output_file(argc, argv);
    if(output_path) {
        if (check_filename_ext(output_path, OUTPUT_FILE_EXT)) {
            printf("Invalid output file extension!\n");
            return 91;
        }

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
    lines = read_input_file(input_file_ptr);
    if(!lines) {
        printf("Error reading input file!\n");
        return 92;
    }

    res_code = process_lines(lines);

    if(!res_code) {
        printf("Error processing lines!\n");
        return 93;
    }

    /* If no output file was specified, print "obrazovka" */
    if (!output_path) {
        printf("obrazovka\n");
    }

    free(input_path);
    fclose(output_file_ptr);
    fclose(input_file_ptr);

    return EXIT_SUCCESS;
}
