#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "file.h"
#include "lp.h"
#include "parse.h"

/* TODO
 * sanity checky
 * check syntaxe (sekce, po end nesmí nic být, povolené operátory)
 * místo return používat exit() a at_exit()
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
    FILE *output_file_ptr = NULL, *input_file_ptr = NULL;
    char *output_path, *input_path;
    SimplexTableau *simplex_tableau;
    SectionBuffers *section_buffers;
    General_vars *general_vars;
    int var_num = 0, subject_to_count;
    int i, j;


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
    section_buffers = create_section_buffers(INITIAL_SIZE);
    if(!section_buffers) {
        return 2;
    }

    read_store_input_file(input_file_ptr, section_buffers);

    pre_parse(section_buffers, &var_num, &subject_to_count);

    simplex_tableau = create_simplex_tableau(subject_to_count, var_num);

    parse_lines(section_buffers, simplex_tableau, &general_vars);
/*
    for (i = 0; i < simplex_tableau->row_count; i++) {
        for (j = 0; j < simplex_tableau->col_count; j++) {
            printf("| %10.4f ", simplex_tableau->tableau[i][j]);
        }
        printf("|\n");
    }

    for (j = 0; j < simplex_tableau->col_count; j++) {
        printf("----------");
    }
    printf("\n");
*/
    simplex(simplex_tableau);

    if(!general_vars) {
        printf("asdashduiash\n");
        return 3;
    }

    print_solution(simplex_tableau, general_vars);

    /* If no output file was specified, print "obrazovka" */
    if (!output_path) {
        printf("obrazovka\n");
    }

    free(input_path);
    fclose(output_file_ptr);
    fclose(input_file_ptr);

    printf("Neumřelo to\n");

    return EXIT_SUCCESS;
}
