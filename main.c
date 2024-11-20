#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "file.h"
#include "lp.h"
#include "parse.h"

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
    int var_num = 0, subject_to_count;
    int i, j;


	if (argc < 2) {
        header();
        help();
        return 3;
    }

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

    pre_parse(section_buffers, &var_num, &subject_to_count);

    simplex_tableau = create_simplex_tableau(subject_to_count, var_num);

    parse_lines(section_buffers, simplex_tableau, &general_vars);

    simplex(simplex_tableau, strcasecmp(simplex_tableau->type, "Minimize") == 0 ? 1 : 0);

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

    printf("Neumřelo to\n");

    return EXIT_SUCCESS;
}
