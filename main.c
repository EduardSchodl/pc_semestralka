#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "File/file.h"
#include "LProblem/lp.h"
#include "Parse/parse.h"
#include "Validate/validate.h"
#include "Bounds/bounds.h"
#include "Memory_manager/memory_manager.h"
#include "Subject_to/subject_to.h"
#include "Objectives/objectives.h"

/* TODO
 * check syntaxe (sekce, po end nesmí nic být, povolené operátory, ...)
 * dodělat kontrolu operátorů, závorky, atd.
 * po free dát null
 * komentáře
 * magické proměnné
 * lepší handlování error codes
 * udělat si funkci, která bude volat parsovací funkce (neplnit rovnou tabulku, až po parse všeho)
 * nazvy promenných by meli brat i napr @ atd, kontrolovat, jestli neobsahují operátory, x1 * x2?
 *
 *
* if (isdigit(*(closing + 1))) {
                double multiplier = strtod(closing + 1, &end_pointer);
                bracket_multiplier= multiplier;
                bracket_multiplier_adjustment = end_pointer - closing - 1;
            } else if ((closing + 1) == '') {
                if (isdigit(*(closing + 2))) {
                    double multiplier = strtod(closing + 2, &end_pointer);
                    bracket_multiplier= multiplier;
                    bracket_multiplier_adjustment = end_pointer - closing - 1;
                }
            }
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

/*
   ____________________________________________________________________________

    int cleanup_and_exit()

    Uvolní všechny dynamicky alokované prostředky a ukončí program s návratovým
    kódem předaným parametrem 'res_code'. Funkce se stará o správné uvolnění
    paměti, ať už program skončí chybou, nebo úspěšně.
   ____________________________________________________________________________
*/
int cleanup_and_exit(int res_code,
                     SectionBuffers *section_buffers,
                     General_vars *general_vars,
                     SimplexTableau *simplex_tableau,
                     Bounds *bounds,
                     double *objective_row,
                     double *solution,
                     FILE *input_file,
                     FILE *output_file) {
    if (section_buffers) free_section_buffers(section_buffers);
    if (general_vars) free_general_vars(general_vars);
    if (simplex_tableau) free_simplex_tableau(simplex_tableau);
    if (bounds) free_bounds(bounds);
    if (objective_row) tracked_free(objective_row);
    if (solution) tracked_free(solution);
    if (input_file) fclose(input_file);
    if (output_file) fclose(output_file);

    /*report_memory_usage();*/

    return res_code;
}


/*
   ____________________________________________________________________________

    int main()

    Hlavní funkce programu. Zpracovává vstupní argumenty, otevírá soubory,
    načítá a zpracovává data z .lp souboru a volá jednotlivé komponenty
    programu a řeší lineární problém pomocí simplexní metody. Nakonec vypisuje
    výsledek buď do konzole, nebo do zadaného výstupního souboru.
   ____________________________________________________________________________
*/
int main(const int argc, char** argv) {
    FILE *output_file_ptr = NULL, *input_file_ptr = NULL;
    char output_path[MAX_PATH_LENGTH] = {0}, input_path[MAX_PATH_LENGTH] = {0};
    SimplexTableau *simplex_tableau = NULL;
    SectionBuffers *section_buffers = NULL;
    General_vars *general_vars = NULL;
    Bounds *bounds = NULL;
    double *objective_row = NULL;
    int res_code = 0;
    double *solution = NULL;

    /* kontrola parametrů příkazové řádky */
    if (argc < 2) {
        header();
        help();
        return 3;
    }

    /* získání cest z příkazové řádky */
    res_code = process_line_args(argc, argv, input_path, output_path);
    if(res_code) {
        return cleanup_and_exit(res_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    if (!*input_path) {
        return cleanup_and_exit(93, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* otevření input souboru .lp */
    res_code = open_file(input_path, "r", &input_file_ptr);
    if (res_code) {
        return cleanup_and_exit(res_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* otevření output souboru */
    if(*output_path) {
        res_code = open_file(output_path, "w", &output_file_ptr);
        if (res_code) {
            return cleanup_and_exit(res_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
                solution, input_file_ptr, output_file_ptr);
        }
    }

    /* vytvoření section_buffers */
    section_buffers = create_section_buffers(INITIAL_SIZE);
    if(!section_buffers) {
        return cleanup_and_exit(93, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* načtení input souboru do section_buffers */
    res_code = load_input_file(input_file_ptr, section_buffers);
    if (res_code) {
        return cleanup_and_exit(res_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* parsování sekce generals */
    res_code = parse_generals(&general_vars, section_buffers->general_lines, section_buffers->general_count);
    if (res_code) {
        return cleanup_and_exit(res_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* vytvoření simplex tabulky */
    simplex_tableau = create_simplex_tableau(section_buffers->subject_to_count, general_vars->num_general_vars);
    if (!simplex_tableau) {
        return cleanup_and_exit(93, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* alokace pole pro účelovou funkci */
    objective_row = (double *)tracked_calloc(simplex_tableau->col_count, sizeof(double));
    if(!objective_row) {
        return cleanup_and_exit(93, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* parsování subject to sekce */
    res_code = parse_subject_to(section_buffers->subject_to_lines, section_buffers->subject_to_count, simplex_tableau, general_vars);
    if (res_code) {
        return cleanup_and_exit(res_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* parsování sekce bounds */
    res_code = parse_bounds(&bounds, general_vars, section_buffers->bounds_lines, section_buffers->bounds_count);
    if (res_code) {
        return cleanup_and_exit(res_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* parsování sekce s účelovou funkcí */
    res_code = parse_objectives(section_buffers->objective_lines, simplex_tableau, general_vars, objective_row, section_buffers->objective_count);
    if (res_code) {
        return cleanup_and_exit(res_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* alokace pole pro výsledek */
    solution = (double *)tracked_calloc(general_vars->num_general_vars, sizeof(double));
    if(!solution) {
        return cleanup_and_exit(res_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* Two-phase simplex */
    res_code = simplex(simplex_tableau, objective_row, general_vars, bounds, solution);
    if (res_code) {
        return cleanup_and_exit(res_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* kontrola nepoužitých proměnných */
    check_unused_variables(general_vars);

    /* výpis do souboru, nebo konzole */
    if (output_file_ptr) {
        write_output_file(output_file_ptr, solution, general_vars);
    }
    else {
        print_solution(general_vars, solution);
    }

    printf("Neumřelo to\n");

    return cleanup_and_exit(EXIT_SUCCESS, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
        solution, input_file_ptr, output_file_ptr);
}