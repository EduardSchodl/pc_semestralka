#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "File/file.h"
#include "LProblem/lp.h"
#include "Parse/parse.h"
#include "Validate/validate.h"
#include "Bounds/bounds.h"

/* TODO
 * check syntaxe (sekce, po end nesmí nic být, povolené operátory, ...)
 * v simplexu checkovat, jestli je v bounds
 * dodělat kontrolu operátorů, závorky, atd.
 * po free dát null
 * zkontrolovat, jestli include s <> je před ""
 * komentáře
 *
 * unbounded.lp nejde?
 * možná proto, že není sekce bounds a program se ji snaží parsovat?
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

int cleanup_and_exit(int res_code,
                     char *input_path, char *output_path,
                     SectionBuffers *section_buffers,
                     General_vars *general_vars,
                     SimplexTableau *simplex_tableau,
                     Bounds *bounds,
                     double *objective_row) {
    if (input_path) free(input_path);
    if (output_path) free(output_path);
    if (section_buffers) free_section_buffers(section_buffers);
    if (general_vars) free_general_vars(general_vars);
    if (simplex_tableau) free_simplex_tableau(simplex_tableau);
    if (bounds) free_bounds(bounds);
    if (objective_row) free(objective_row);

    return res_code;
}

int main(const int argc, char** argv) {
    FILE *output_file_ptr = NULL, *input_file_ptr = NULL;
    char *output_path = NULL, *input_path = NULL;
    SimplexTableau *simplex_tableau = NULL;
    SectionBuffers *section_buffers = NULL;
    General_vars *general_vars = NULL;
    Bounds *bounds = NULL;
    double *objective_row = NULL;
    int res_code = 0;

	if (argc < 2) {
        header();
        help();
        return 3;
    }

    /* získání cesty k output souboru z parametrů příkazové řádky */
    output_path = get_output_file(argc, argv);

    /* získání cesty k input souboru z parametrů příkazové řádky */
    input_path = get_input_file(argc, argv);
    if (!input_path) {
        return cleanup_and_exit(93, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
    }

    /* otevření input souboru .lp */
    res_code = open_file(input_path, "r", &input_file_ptr);
    if (res_code) {
        return cleanup_and_exit(res_code, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
    }

    /* vytvoření section_buffers */
    section_buffers = create_section_buffers(INITIAL_SIZE);
    if(!section_buffers) {
        return cleanup_and_exit(93, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
    }

    /* načtení input souboru do section_buffers */
    res_code = load_input_file(input_file_ptr, section_buffers);
    if (res_code) {
        return cleanup_and_exit(res_code, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
    }

    /* uzavření input souboru */
    fclose(input_file_ptr);

    /* parsování sekce generals */
    res_code = parse_generals(&general_vars, section_buffers->general_lines, section_buffers->general_count);
    if (res_code) {
        return cleanup_and_exit(res_code, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
    }

    /* vytvoření simplex tabulky */
    simplex_tableau = create_simplex_tableau(section_buffers->subject_to_count, general_vars->num_general_vars);
    if (!simplex_tableau) {
        return cleanup_and_exit(93, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
    }

    /* alokace pole pro účelovou funkci */
    objective_row = (double *)calloc(simplex_tableau->col_count, sizeof(double));
    if(!objective_row) {
        return cleanup_and_exit(93, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
    }

    /* parsování subject to sekce */
    res_code = parse_subject_to(section_buffers->subject_to_lines, section_buffers->subject_to_count, simplex_tableau, general_vars);
    if (res_code) {
        return cleanup_and_exit(res_code, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
    }

    /* parsování sekce bounds */
    res_code = parse_bounds(&bounds, general_vars, section_buffers->bounds_lines, section_buffers->bounds_count);
    if (res_code) {
        return cleanup_and_exit(res_code, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
    }

    /* parsování sekce s účelovou funkcí */
    res_code = parse_objectives(section_buffers->objective_lines, simplex_tableau, general_vars, objective_row, section_buffers->objective_count);
    if (res_code) {
        return cleanup_and_exit(res_code, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
    }

    /* Two-phase simplex */
    res_code = simplex(simplex_tableau, objective_row, general_vars, bounds);
    if (res_code) {
        return cleanup_and_exit(res_code, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
    }

    /* kontrola nepoužitých proměnných */
    check_unused_variables(general_vars);

    /* výpis do souboru, nebo konzole */
    if (output_path) {
        res_code = open_file(output_path, "w", &output_file_ptr);
        if (res_code) {
            return cleanup_and_exit(res_code, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
        }

        write_output_file(output_file_ptr, simplex_tableau, general_vars);

        fclose(output_file_ptr);
    }
    else {
        print_solution(simplex_tableau, general_vars);
    }

    printf("Neumřelo to\n");

    return cleanup_and_exit(EXIT_SUCCESS, input_path, output_path,
                                section_buffers, general_vars, simplex_tableau, bounds, objective_row);
}