#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __linux__
#include <strings.h>
#endif
#include <string.h>
#include "lp.h"
#include "../Generals/generals.h"
#include "../Bounds/bounds.h"
#include "../Memory_manager/memory_manager.h"
#include "../Parse/parse.h"
#include "../Consts/error_codes.h"
#include "../Consts/constants.h"

int simplex(Simplex_Tableau *tableau, double objective_row[], General_vars *general_vars, Bounds *bounds,
            double *solution) {
    int i;
    int num_artificial_vars;

    /* sanity check */
    if (!tableau || !objective_row || !general_vars || !bounds) {
        return SANITY_CHECK_ERROR;
    }

    /* počet umělých proměnných odpovídá počtu omezení */
    num_artificial_vars = tableau->row_count - 1;

    /*print_tableau(tableau);*/

    /* fáze jedna Simplex metody */
    if (simplex_phase_one(tableau) != 0) {
        printf("No feasible solution exists.\n");
        return NO_FEASIBLE_SOLUTION;
    }

    /* odstranění umělých proměnných z tabulky */
    if (remove_artificial_variables(tableau, num_artificial_vars)) {
        printf("Error during artificial variables removal!\n");
        return RUNTIME_ERROR;
    }

    /* nastavení řádku cílové funkce */
    for (i = 0; i < tableau->col_count; i++) {
        tableau->tableau[tableau->row_count - 1][i] = -objective_row[i];
    }

    /*print_tableau(tableau);*/

    /* fáze dvě Simplex metody */
    if (simplex_phase_two(tableau, general_vars->num_general_vars) != 0) {
        printf("Objective function is unbounded.\n");
        return FUNCTION_UNBOUNDED_ERROR;
    }

    /*print_tableau(tableau);*/

    /* kontrola, zda řešení splňuje zadané meze */
    if (check_solution_bounds(tableau, general_vars, bounds) != 0) {
        printf("Optimal solution is out of bounds.\n");
        return SOLUTION_OUT_OF_BOUNDS;
    }

    print_tableau(tableau);

    /* extrakce řešení z tabulky */
    extract_solution(tableau, general_vars, solution);

    return 0;
}

int remove_artificial_variables(Simplex_Tableau *tableau, int num_artificial_vars) {
    int i, j, k;
    double *new_row;
    int new_col_count;

    /* sanity check */
    if (!tableau) {
        return 1;
    }

    /* výpočet nového počtu sloupců */
    new_col_count = tableau->col_count - num_artificial_vars;

    /* aktualizace tabulky bez umělých proměnných */
    for (i = 0; i < tableau->row_count; i++) {
        new_row = (double *) tracked_malloc(new_col_count * sizeof(double));
        if (!new_row) {
            printf("Memory allocation error during artificial variable removal.\n");
            return 1;
        }

        k = 0;
        for (j = 0; j < tableau->col_count; j++) {
            if (j < tableau->col_count - num_artificial_vars - 1 || j == tableau->col_count - 1) {
                new_row[k++] = tableau->tableau[i][j];
            }
        }

        tracked_free(tableau->tableau[i]);
        tableau->tableau[i] = new_row;
    }

    tableau->col_count = new_col_count;

    return 0;
}

Simplex_Tableau *create_simplex_tableau(int num_constraints, int num_variables) {
    Simplex_Tableau *temp;
    int i, j;
    int num_slacks = num_constraints; /* počet slack proměnných */
    int num_artificials = num_constraints; /* počet umělých proměnných */
    int num_cols = num_variables + num_slacks + num_artificials + 1; /* celkový počet sloupců */
    int num_rows = num_constraints + 1; /* počet řádků */

    temp = (Simplex_Tableau *) tracked_malloc(sizeof(Simplex_Tableau));
    if (!temp) {
        return NULL;
    }

    temp->row_count = num_rows;
    temp->col_count = num_cols;

    temp->tableau = (double **) tracked_malloc(num_rows * sizeof(double *));
    if (!temp->tableau) {
        tracked_free(temp);
        return NULL;
    }

    temp->type = tracked_malloc(MAX_LINE_SIZE * sizeof(char *));
    if (!temp->type) {
        printf("Memory allocation failed for tableau->type\n");
        return NULL;
    }

    temp->basic_vars = (int *) tracked_malloc(num_variables * sizeof(int));
    if (!temp->basic_vars) {
        printf("Memory allocation failed for tableau->basic_vars\n");
        return NULL;
    }

    /* nastavení základních proměnných na -1 */
    for (i = 0; i < num_rows; i++) {
        temp->basic_vars[i] = -1;
    }

    for (i = 0; i < num_rows; i++) {
        temp->tableau[i] = tracked_calloc(num_cols, sizeof(double));
        if (!temp->tableau[i]) {
            for (j = 0; j < i; j++) {
                tracked_free(temp->tableau[j]);
            }
            tracked_free(temp->tableau);
            tracked_free(temp);
            return NULL;
        }
    }

    return temp;
}

int check_solution_bounds(Simplex_Tableau *tableau, General_vars *general_vars, Bounds *bounds) {
    int i;
    double value, lower_bound, upper_bound;

    /* sanity check */
    if (!tableau || !general_vars || !bounds) {
        return 1;
    }

    /* procházení všech proměnných a kontrola, zda splňují své meze */
    for (i = 0; i < general_vars->num_general_vars; i++) {
        if (i < tableau->row_count - 1) {
            value = tableau->tableau[i][tableau->col_count - 1];
        } else {
            value = 0.0; /* hodnota proměnných mimo tabulku je 0 */
        }

        lower_bound = bounds->lower_bound[i];
        upper_bound = bounds->upper_bound[i];

        /* kontrola, zda hodnota proměnné spadá do příslušných mezí */
        if (value < lower_bound - 1e-6 || value > upper_bound + 1e-6) {
            /*printf("Variable %s is out of bounds: Value = %.6f, Bounds = [%.6f, %.6f]\n",
                   general_vars->general_vars[i], value, lower_bound, upper_bound);*/
            return 1;
        }
    }

    return 0;
}

void perform_pivoting(Simplex_Tableau *tableau, int pivot_row, int pivot_col) {
    int i, j;
    double pivot_element, factor;

    /* sanity check */
    if (!tableau) {
        return;
    }

    /* získání pivotního prvku */
    pivot_element = tableau->tableau[pivot_row][pivot_col];
    /*printf("coords: %d,%d\n", pivot_row, pivot_col);*/

    /* normalizace pivotního řádku */
    for (j = 0; j < tableau->col_count; j++) {
        tableau->tableau[pivot_row][j] /= pivot_element;
    }

    /* eliminace ostatních řádků pomocí pivotního prvku */
    for (i = 0; i < tableau->row_count; i++) {
        if (i != pivot_row) {
            factor = tableau->tableau[i][pivot_col];
            for (j = 0; j < tableau->col_count; j++) {
                tableau->tableau[i][j] -= factor * tableau->tableau[pivot_row][j];
            }
        }
    }
}

int is_basic_variable(const Simplex_Tableau *tableau, int col_index) {
    int i;
    int one_count = 0;

    /* kontrola, zda daný sloupec obsahuje pouze jednu hodnotu */
    for (i = 0; i < tableau->row_count - 1; i++) {
        if (tableau->tableau[i][col_index] == 1.0) {
            one_count++;
        } else if (tableau->tableau[i][col_index] != 0.0) {
            return 0; /* pokud obsahuje jiný nenulový prvek, není základní */
        }
    }

    return (one_count == 1); /* sloupec je základní, pokud obsahuje jednu hodnotu */
}

void extract_solution(Simplex_Tableau *tableau, const General_vars *general_vars, double *solution) {
    int i;
    int num_vars;

    /* sanity check */
    if (!tableau || !general_vars || !solution) {
        return;
    }

    num_vars = general_vars->num_general_vars;

    /* inicializace řešení na 0 */
    for (i = 0; i < num_vars; i++) {
        solution[i] = 0.0;
    }

    /* finální eliminace pomocí Gaussovy metody */
    for (i = 0; i < tableau->row_count - 1; i++) {
        if (tableau->tableau[i][i] == 0.0) {
            tableau->basic_vars[i] = -1;
        }
        else if(!is_basic_variable(tableau, i)) {
            perform_pivoting(tableau, i, i);
            tableau->basic_vars[i] = i;
            print_tableau(tableau);
        }
    }

    /* uložení řešení */
    for (i = 0; i < num_vars; i++) {
        if (tableau->basic_vars[i] != -1 && general_vars->used_vars[i]) {
            solution[tableau->basic_vars[i]] = tableau->tableau[i][tableau->col_count - 1];
        }
    }
}

void print_solution(General_vars *general_vars, double *solution) {
    int i;

    /* sanity check */
    if (!general_vars) {
        return;
    }

    /* výpis řešení */
    for (i = 0; i < general_vars->num_general_vars; i++) {
        if (general_vars->used_vars[i]) {
            printf("%s = %0.4f\n", general_vars->general_vars[i], solution[i]);
        }
    }
}

int find_pivot_row(const Simplex_Tableau *tableau, const int col_index) {
    int i;
    int smallest_quotient_row = -1;
    double smallest_ratio = DBL_MAX;
    double ratio;
    double element;

    /* sanity check */
    if (!tableau) {
        return -1;
    }

    /* hledání řádku s nejmenším podílem pravé strany a pivotního sloupce */
    for (i = 0; i < tableau->row_count - 1; i++) {
        element = tableau->tableau[i][col_index];
        if (element > 0) {
            ratio = tableau->tableau[i][tableau->col_count - 1] / element;
            if (ratio < smallest_ratio) {
                smallest_ratio = ratio;
                smallest_quotient_row = i;
            }
        }
    }

    return smallest_quotient_row;
}

int find_pivot_col(const Simplex_Tableau *tableau, int minimization) {
    int i;
    int pivot_col = -1;
    double best_value = minimization ? -DBL_MAX : DBL_MAX;
    double current_value;

    /* sanity check */
    if (!tableau) {
        return -1;
    }

    /* hledání sloupce s nejmenší (maximalizační) nebo největší (minimalizační) hodnotou */
    for (i = 0; i < tableau->col_count - 1; i++) {
        current_value = tableau->tableau[tableau->row_count - 1][i];

        if (minimization) {
            if (current_value > best_value) {
                best_value = current_value;
                pivot_col = i;
            }
        } else {
            if (current_value < best_value) {
                best_value = current_value;
                pivot_col = i;
            }
        }
    }

    /* pokud neexistuje vhodný pivotní sloupec, metoda končí */
    if (!minimization && best_value >= 0) {
        return -1;
    }

    if (minimization && best_value <= 0) {
        return -1;
    }

    /*printf("best_value: %f\n", best_value);*/

    return pivot_col;
}

void free_simplex_tableau(Simplex_Tableau *tableau) {
    int i;

    /* sanity check */
    if (!tableau) {
        return;
    }

    /* uvolnění struktury Simplex_Tableau */
    if (tableau->type) {
        tracked_free(tableau->type);
    }

    if (tableau->tableau) {
        for (i = 0; i < tableau->row_count; i++) {
            if (tableau->tableau[i]) {
                tracked_free(tableau->tableau[i]);
            }
        }
        tracked_free(tableau->tableau);
    }

    if (tableau->basic_vars) {
        tracked_free(tableau->basic_vars);
    }

    tracked_free(tableau);
}

void print_tableau(Simplex_Tableau *simplex_tableau) {
    int i, j;

    /* sanity check */
    if (!simplex_tableau) {
        return;
    }

    /* výpis obsahu tabulky */
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
}

int simplex_phase_one(Simplex_Tableau *tableau) {
    int pivot_col;
    int pivot_row;
    int i;
    int all_non_negative = 0;

    /* sanity check */
    if (!tableau) {
        return 1;
    }

    /* iterace fáze 1 */
    while (1) {
        /* kontrola ukončení, pokud je poslední prvek posledního řádku 0 */
        if (tableau->tableau[tableau->row_count - 1][tableau->col_count - 1] == 0) {
            return 0;
        }

        /* kontrola, zda je problém neřešitelný */
        if (all_non_negative) {
            if (tableau->tableau[tableau->row_count - 1][tableau->col_count - 1] > 1e-6) {
                /*printf("The problem is infeasible.\n");*/
                return 1;
            }
            /*printf("Phase One complete. Feasible solution found.\n");*/
            return 0;
        }

        /* hledání pivotního sloupce */
        pivot_col = find_pivot_col(tableau, 1);
        if (pivot_col == -1) {
            /*printf("Error: No pivot column found.\n");*/
            return 1;
        }

        /* hledání pivotního řádku */
        pivot_row = find_pivot_row(tableau, pivot_col);
        if (pivot_row == -1) {
            /*printf("Objective function is unbounded or numerically unstable.\n");*/
            return 1;
        }

        /* nastavení základní proměnné a provedení pivotování */
        tableau->basic_vars[pivot_row] = pivot_col;

        perform_pivoting(tableau, pivot_row, pivot_col);

        /* kontrola, zda jsou všechny hodnoty v posledním řádku kladné */
        all_non_negative = 1;
        for (i = 0; i < tableau->col_count - 1; i++) {
            if (tableau->tableau[tableau->row_count - 1][i] < -1e-6) {
                all_non_negative = 0;
                break;
            }
        }

        /*print_tableau(tableau);*/
    }
}

double my_fabs(double x) {
    /* absolutní hodnota */
    return x < 0 ? -x : x;
}

int has_nonzero_in_objective_row(Simplex_Tableau *tableau, int num_general_vars) {
    int i;

    /* sanity check */
    if (!tableau) {
        return 1;
    }

    /* kontrola, zda v řádku cílové funkce existují nenulové hodnoty */
    for (i = 0; i < num_general_vars; i++) {
        if (my_fabs(tableau->tableau[tableau->row_count - 1][i]) > 1e-6) {
            return 1;
        }
    }

    return 0;
}

int simplex_phase_two(Simplex_Tableau *tableau, int num_general_vars) {
    int pivot_col;
    int pivot_row;
    int i;
    int all_non_positive;

    /* sanity check */
    if (!tableau) {
        return 1;
    }

    /* iterace fáze 2 */
    while (1) {
        all_non_positive = 1;

        /* kontrola, zda jsou všechny hodnoty v objektivní řádce nezáporné */
        for (i = 0; i < tableau->col_count - 1; i++) {
            if (tableau->tableau[tableau->row_count - 1][i] < 1e-6) {
                all_non_positive = 0;
                break;
            }
        }

        /* pokud jsou všechny hodnoty nezáporné, kontrolujeme přítomnost nenulových hodnot */
        if (!all_non_positive) {
            if (has_nonzero_in_objective_row(tableau, num_general_vars)) {
                pivot_col = find_pivot_col(tableau, 0);
                if (pivot_col == -1) {
                    /* Nelze najít pivotní sloupec, optimalizace je dokončena */
                    /*printf("Error: No pivot column found.\n");*/
                    return 0;
                }
            } else {
                /* Optimalizace dokončena, nalezeno řešení */
                /*printf("Phase Two complete. Feasible solution found.\n");*/
                return 0;
            }
        }

        /* hledání pivotního sloupce */
        pivot_col = find_pivot_col(tableau, 0);
        if (pivot_col == -1) {
            /*printf("The problem is unbounded.\n");*/
            return 0;
        }

        /* hledání pivotního řádku */
        pivot_row = find_pivot_row(tableau, pivot_col);
        if (pivot_row == -1) {
            /*printf("The problem is unbounded.\n");*/
            return 1;
        }

        /* nastavení základní proměnné a provedení pivotování */
        tableau->basic_vars[pivot_row] = pivot_col;

        perform_pivoting(tableau, pivot_row, pivot_col);

        print_tableau(tableau);
    }
}

int insert_constraints_into_row(char *expression, General_vars *general_vars, double *arr) {
    char *token;
    double coefficient;
    int var_index;
    char variable[MAX_VAR_NAME] = {0};

    /* sanity check */
    if (!expression || !arr || !general_vars) {
        return SANITY_CHECK_ERROR;
    }

    /* tokenizace řetězce podle znaku '+' */
    token = strtok(expression, "+");
    while (token != NULL) {
        remove_spaces(token); /* odstranění mezer z tokenu */

        /* zpracování tokenu a získání proměnné a koeficientu */
        if (strlen(token) > 0 && extract_variable_and_coefficient(token, variable, &coefficient) == 0) {
            /* hledání indexu proměnné */
            var_index = get_var_index(general_vars, variable);
            if (var_index == -1) {
                /* neznámá proměnná */
                printf("Unknown variable '%s'!\n", variable);
                return UNKNOWN_VARIABLE;
            }

            /* označení proměnné jako použité */
            general_vars->used_vars[var_index] = 1;

            /* uložení koeficientu do pole */
            arr[var_index] = coefficient;
        } else {
            return PARSING_ERROR;
        }

        token = strtok(NULL, "+");
    }

    return 0;
}
