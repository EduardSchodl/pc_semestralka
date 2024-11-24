#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "file.h"
#include "lp.h"
#include "parse.h"
#include "Bounds/bounds.h"

/* TODO
 * sanity checky
 * dodat uvolnění, když předčasně program skončí
 * check syntaxe (sekce, po end nesmí nic být, povolené operátory, ...)
 * místo return používat exit() a at_exit()
 * v simplexu checkovat, jestli je v bounds
 * celkově překopat kód, aby byl hezčí (pokaždé zkusit v linuxu)
 * dodělat funkci na zápis do souboru
 * dodělat kontrolu operátorů, závorky, atd.
 * po free dát null
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
    Bounds *bounds;
    double *objective_row;
    int i = 0;

	if (argc < 2) {
        header();
        help();
        return 3;
    }

    /* spojit open do jednoho? lepší handlování chyb */
    output_path = get_output_file(argc, argv);
    output_file_ptr = open_output_file(output_path);

    input_path = get_input_file(argc, argv);
    input_file_ptr = open_input_file(input_path);

    /* logika aplikace */
    section_buffers = create_section_buffers(INITIAL_SIZE);
    if(!section_buffers) {
        exit(93);
    }

    read_store_input_file(input_file_ptr, section_buffers);

    /* parsing generals */
    pre_parse(section_buffers, &general_vars);

    /* vytvořim tabulku */
    simplex_tableau = create_simplex_tableau(section_buffers->subject_to_count, general_vars->num_general_vars);
    if (!simplex_tableau) {
        return 93;
    }

    objective_row = (double *)calloc(simplex_tableau->col_count, sizeof(double));
    if(!objective_row) {
        return 93;
    }

    parse_subject_to(section_buffers->subject_to_lines, section_buffers->subject_to_count, simplex_tableau, general_vars);

    /* parsing objectives a bounds a do proměnné uloží objective row místo rovnou do tabulky */
    parse_lines(section_buffers, simplex_tableau, general_vars, &bounds, objective_row);

    /* bude se předávát pole objectives zvlášť? */
    simplex(simplex_tableau, objective_row, general_vars->num_general_vars);

    print_solution(simplex_tableau, general_vars);

    /* If no output file was specified, print "obrazovka" */
    if (!output_path) {
        printf("obrazovka\n");
    }
    else {
        fclose(output_file_ptr);
    }

    free(input_path);
    free(output_path);
    fclose(input_file_ptr);

    free_section_buffers(section_buffers);
    free_simplex_tableau(simplex_tableau);
    free_general_vars(general_vars);
    free(objective_row);

    printf("Neumřelo to\n");

    return EXIT_SUCCESS;
}