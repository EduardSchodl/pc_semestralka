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
#include "Consts/error_codes.h"
#include "Consts/constants.h"

/* TODO
 * po free dát null
 * magické proměnné
 * udělat si funkci, která bude volat parsovací funkce (neplnit rovnou tabulku, až po parse všeho)
 *
 */

/*
   ____________________________________________________________________________

    void header()

    Vypíše do konzole hlavičku programu - konkrétně název programu
    a informace o autorských právech a verzi.
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

    Vytiskne krátkou nápovědu. Ukáže, jak program používat, jaké jsou
    možnosti příkazového řádku atd.
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

    if(res_code == 11) {
        printf("Syntax error!\n");
    }

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
    int result_code = 0;
    double *solution = NULL;

    /* kontrola parametrů příkazové řádky */
    if (argc < 2) {
        header();
        help();
        return INVALID_ARGUMENTS_COUNT;
    }

    /* získání cest z příkazové řádky */
    result_code = process_line_args(argc, argv, input_path, output_path);
    if(result_code) {
        return cleanup_and_exit(result_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* kontrola, jestli byl zadán vstupní soubor */
    if (!*input_path) {
        return cleanup_and_exit(PARSING_ERROR, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* otevření input souboru */
    result_code = open_file(input_path, "r", &input_file_ptr);
    if (result_code) {
        return cleanup_and_exit(result_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* otevření output souboru */
    if(*output_path) {
        result_code = open_file(output_path, "w", &output_file_ptr);
        if (result_code) {
            return cleanup_and_exit(result_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
                solution, input_file_ptr, output_file_ptr);
        }
    }

    /* vytvoření section_buffers */
    section_buffers = create_section_buffers(INITIAL_SIZE);
    if(!section_buffers) {
        return cleanup_and_exit(MEMORY_ERROR, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* načtení input souboru do section_buffers */
    result_code = load_input_file(input_file_ptr, section_buffers);
    if (result_code) {
        return cleanup_and_exit(result_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* parsování sekce generals */
    result_code = parse_generals(&general_vars, section_buffers->general_lines, section_buffers->general_count);
    if (result_code) {
        return cleanup_and_exit(result_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* vytvoření simplex tabulky */
    simplex_tableau = create_simplex_tableau(section_buffers->subject_to_count, general_vars->num_general_vars);
    if (!simplex_tableau) {
        return cleanup_and_exit(MEMORY_ERROR, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* alokace pole pro účelovou funkci */
    objective_row = (double *)tracked_calloc(simplex_tableau->col_count, sizeof(double));
    if(!objective_row) {
        return cleanup_and_exit(MEMORY_ERROR, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* parsování subject to sekce */
    result_code = parse_subject_to(section_buffers->subject_to_lines, section_buffers->subject_to_count, simplex_tableau, general_vars);
    if (result_code) {
        return cleanup_and_exit(result_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* parsování sekce bounds */
    result_code = parse_bounds(&bounds, general_vars, section_buffers->bounds_lines, section_buffers->bounds_count);
    if (result_code) {
        return cleanup_and_exit(result_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* parsování sekce s účelovou funkcí */
    result_code = parse_objectives(section_buffers->objective_lines, simplex_tableau, general_vars, objective_row, section_buffers->objective_count);
    if (result_code) {
        return cleanup_and_exit(result_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* alokace pole pro výsledeků */
    solution = (double *)tracked_calloc(general_vars->num_general_vars, sizeof(double));
    if(!solution) {
        return cleanup_and_exit(result_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
            solution, input_file_ptr, output_file_ptr);
    }

    /* Two-phase simplex */
    result_code = simplex(simplex_tableau, objective_row, general_vars, bounds, solution);
    if (result_code) {
        return cleanup_and_exit(result_code, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
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

    /* program končí úspěšně */
    return cleanup_and_exit(EXIT_SUCCESS, section_buffers, general_vars, simplex_tableau, bounds, objective_row,
        solution, input_file_ptr, output_file_ptr);
}