#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "lp.h"
#include "../File/file.h"
#include "../Generals/generals.h"
#include "../Bounds/bounds.h"
#include "../Memory_manager/memory_manager.h"
#include "../Parse/parse.h"

int simplex(SimplexTableau *tableau, double objective_row[], General_vars *general_vars, Bounds *bounds, double *solution) {
    int i;
    int num_artificial_vars;

    if(!tableau || !objective_row || !general_vars || !bounds) {
        return 93;
    }

    num_artificial_vars = tableau->row_count - 1;

    /*print_tableau(tableau);*/
    if (simplex_phase_one(tableau) != 0) {
        printf("No feasible solution exists.\n" );
        return 21;
    }

    if(remove_artificial_variables(tableau, num_artificial_vars)) {
        printf("Error during artificial variables removal!\n");
        return 93;
    }

    for (i = 0; i < tableau->col_count; i++) {
        if(strcasecmp(tableau->type, "Minimize") == 0) {
            tableau->tableau[tableau->row_count - 1][i] = -objective_row[i];
        }
        else {
            tableau->tableau[tableau->row_count - 1][i] = -objective_row[i];
        }
    }

    print_tableau(tableau);

    if (simplex_phase_two(tableau, general_vars->num_general_vars) != 0) {
        printf("Objective function is unbounded.\n");
        return 20;
    }

    print_tableau(tableau);

    if (check_solution_bounds(tableau, general_vars, bounds) != 0) {
        printf("Optimal solution is out of bounds.\n");
        return 22;
    }

    extract_solution(tableau, general_vars, solution);

    return 0;
}

int remove_artificial_variables(SimplexTableau *tableau, int num_artificial_vars) {
    int i, j, k;
    double *new_row;
    int new_col_count;

    if(!tableau) {
        return 1;
    }

    new_col_count = tableau->col_count - num_artificial_vars;

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

SimplexTableau *create_simplex_tableau(int num_constraints, int num_variables) {
    SimplexTableau *temp;
    int i, j;
    int num_slacks = num_constraints;
    int num_artificials = num_constraints;
    int num_cols = num_variables + num_slacks + num_artificials + 1;
    int num_rows = num_constraints + 1;

    temp = (SimplexTableau *) tracked_malloc(sizeof(SimplexTableau));
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

    temp->type = tracked_malloc(LINE_MAX_SIZE * sizeof(char *));
    if (!temp->type) {
        printf("Memory allocation failed for tableau->type\n");
        return NULL;
    }

    temp->basic_vars = (int *)tracked_malloc(num_variables * sizeof(int));
    if (!temp->basic_vars) {
        printf("Memory allocation failed for tableau->basic_vars\n");
        return NULL;
    }

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

int check_solution_bounds(SimplexTableau *tableau, General_vars *general_vars, Bounds *bounds) {
    int i;
    double value, lower_bound, upper_bound;

    if (!tableau || !general_vars || !bounds) {
        return 1;
    }

    for (i = 0; i < general_vars->num_general_vars; i++) {
        if(i < tableau->row_count-1) {
            value = tableau->tableau[i][tableau->col_count - 1];
        }
        else {
            value = 0.0;
        }

        lower_bound = bounds->lower_bound[i];
        upper_bound = bounds->upper_bound[i];

        if (value < lower_bound - 1e-6 || value > upper_bound + 1e-6) {
            /*printf("Variable %s is out of bounds: Value = %.6f, Bounds = [%.6f, %.6f]\n",
                   general_vars->general_vars[i], value, lower_bound, upper_bound);*/
            return 1;
        }
    }

    return 0;
}

void perform_pivoting(SimplexTableau *tableau, int pivot_row, int pivot_col) {
    int i, j;
    double pivot_element, factor;

    if(!tableau) {
        return;
    }

    pivot_element = tableau->tableau[pivot_row][pivot_col];
    printf("coords: %d,%d\n", pivot_row, pivot_col);

    for (j = 0; j < tableau->col_count; j++) {
        tableau->tableau[pivot_row][j] /= pivot_element;
    }

    for (i = 0; i < tableau->row_count; i++) {
        if (i != pivot_row) {
            factor = tableau->tableau[i][pivot_col];
            for (j = 0; j < tableau->col_count; j++) {
                tableau->tableau[i][j] -= factor * tableau->tableau[pivot_row][j];
            }
        }
    }
}

int is_basic_variable(const SimplexTableau *tableau, int col_index) {
    int i;
    int one_count = 0;

    for (i = 0; i < tableau->row_count - 1; i++) {
        if (tableau->tableau[i][col_index] == 1.0) {
            one_count++;
        } else if (tableau->tableau[i][col_index] != 0.0) {
            return 0;
        }
    }

    return (one_count == 1);
}

void extract_solution(SimplexTableau *tableau, const General_vars *general_vars, double *solution) {
    int i, j, basic_var;
    int num_vars;

    if (!tableau || !general_vars || !solution) {
        return;
    }

    num_vars = general_vars->num_general_vars;

    for (i = 0; i < num_vars; i++) {
        solution[i] = 0.0;
    }

    for(i = 0; i < tableau->row_count - 1; i++) {
        if(tableau->tableau[i][i] != 0.0 && !is_basic_variable(tableau, i)) {
            perform_pivoting(tableau, i, i);
            print_tableau(tableau);
        }
    }

    for (i = 0; i < general_vars->num_general_vars; i++) {
        if(general_vars->used_vars[i]) {
            basic_var = -1;
            for (j = 0; j < tableau->row_count-1; j++) {
                if(i <= tableau->row_count - 1) {
                    if (tableau->tableau[i][j] == 1.0) {
                        basic_var = j;
                        break;
                    }
                }
            }

            if (basic_var != -1) {
                solution[basic_var] = tableau->tableau[i][tableau->col_count - 1];
            }
        }
    }
}


void print_solution(General_vars *general_vars, double *solution) {
    int i;

    for (i = 0; i < general_vars->num_general_vars; i++) {
        if(general_vars->used_vars[i]) {
            printf("%s = %0.4f\n", general_vars->general_vars[i], solution[i]);
        }
    }
}

int find_pivot_row(const SimplexTableau *tableau, const int col_index) {
    int i;
    int smallest_quotient_row = -1;
    double smallest_ratio = DBL_MAX;
    double ratio;
    double element;

    if(!tableau) {
        return -1;
    }

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

int find_pivot_col(const SimplexTableau *tableau, int minimization) {
    int i;
    int pivot_col = -1;
    double best_value = minimization ? -DBL_MAX : DBL_MAX;
    double current_value;

    if(!tableau) {
        return -1;
    }

    for (i = 0; i < tableau->col_count - 1; i++) {
        current_value = tableau->tableau[tableau->row_count - 1][i];

        if (minimization) {
            if (current_value > best_value) {
                best_value = current_value;
                pivot_col = i;
            }
        }
        else {
            if (current_value < best_value) {
                best_value = current_value;
                pivot_col = i;
            }
        }
    }

    if (!minimization && best_value >= 0) {
        return -1;
    }

    if (minimization && best_value <= 0) {
        return -1;
    }

    /*printf("best_value: %f\n", best_value);*/

    return pivot_col;
}

void free_simplex_tableau(SimplexTableau *tableau) {
    int i;

    if (!tableau) {
        return;
    }

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

    if(tableau->basic_vars) {
        tracked_free(tableau->basic_vars);
    }

    tracked_free(tableau);
}

void print_tableau(SimplexTableau *simplex_tableau) {
    int i, j;

    if(!simplex_tableau) {
        return;
    }

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

int simplex_phase_one(SimplexTableau *tableau) {
    int pivot_col;
    int pivot_row;
    int i;
    int all_non_negative = 0;

    if(!tableau) {
        return 1;
    }

    while (1) {
        if(tableau->tableau[tableau->row_count-1][tableau->col_count-1] == 0) {
            return 0;
        }

        if (all_non_negative) {
            if (tableau->tableau[tableau->row_count - 1][tableau->col_count - 1] > 1e-6) {
                printf("The problem is infeasible.\n");
                return 1;
            }
            /*printf("Phase One complete. Feasible solution found.\n");*/
            return 0;
        }

        pivot_col = find_pivot_col(tableau, 1);
        if (pivot_col == -1) {
            /*printf("Error: No pivot column found.\n");*/
            return 1;
        }

        pivot_row = find_pivot_row(tableau, pivot_col);
        if (pivot_row == -1) {
            /*printf("Objective function is unbounded or numerically unstable.\n");*/
            return 1;
        }

        tableau->basic_vars[pivot_row] = pivot_col;

        perform_pivoting(tableau, pivot_row, pivot_col);

        all_non_negative = 1;
        for (i = 0; i < tableau->col_count - 1; i++) {
            if (tableau->tableau[tableau->row_count - 1][i] < -1e-6) {
                all_non_negative = 0;
                break;
            }
        }

        print_tableau(tableau);
    }
}

double my_fabs(double x) {
    return x < 0 ? -x : x;
}

int has_nonzero_in_objective_row(SimplexTableau *tableau, int num_general_vars) {
    int i;

    if(!tableau) {
        return 1;
    }

    for (i = 0; i < num_general_vars; i++) {
        if (my_fabs(tableau->tableau[tableau->row_count - 1][i]) > 1e-6) {
            return 1;
        }
    }

    return 0;
}

int simplex_phase_two(SimplexTableau *tableau, int num_general_vars) {
    int pivot_col;
    int pivot_row;
    double pivot, factor;
    int i, j;

    if(!tableau) {
        return 1;
    }

    while (1) {
        int all_non_positive = 1;
        for (i = 0; i < tableau->col_count - 1; i++) {
            if (tableau->tableau[tableau->row_count - 1][i] < 1e-6) {
                all_non_positive = 0;
                break;
            }
        }

        if (!all_non_positive) {
            if (has_nonzero_in_objective_row(tableau, num_general_vars)) {
                pivot_col = find_pivot_col(tableau, 0);
                if (pivot_col == -1) {
                    /*printf("Error: No pivot column found.\n");*/
                    return 0;
                }
            } else {
                /*printf("Phase Two complete. Feasible solution found.\n");*/
                return 0;
            }
        }


        pivot_col = find_pivot_col(tableau, 0);
        if (pivot_col == -1) {
            printf("The problem is unbounded.\n");
            return 0;
        }

        pivot_row = find_pivot_row(tableau, pivot_col);
        if (pivot_row == -1) {
            printf("The problem is unbounded.\n");
            return 1;
        }

        pivot = tableau->tableau[pivot_row][pivot_col];

        for (j = 0; j < tableau->col_count; j++) {
            tableau->tableau[pivot_row][j] /= pivot;
        }

        for (i = 0; i < tableau->row_count; i++) {
            if (i != pivot_row) {
                factor = tableau->tableau[i][pivot_col];
                for (j = 0; j < tableau->col_count; j++) {
                    tableau->tableau[i][j] -= factor * tableau->tableau[pivot_row][j];
                }
            }
        }

        print_tableau(tableau);
    }
}

int insert_constraints_into_row(char *expression, General_vars *general_vars, double *arr) {
    char *token;
    double coefficient;
    int var_index;
    char variable[64] = {0};

    if(!expression || !arr || !general_vars) {
        return 93;
    }

    token = strtok(expression, "+");
    while (token != NULL) {
        remove_spaces(token);
        if (strlen(token) > 0 && extract_variable_and_coefficient(token, variable, &coefficient) == 0) {
            var_index = get_var_index(general_vars, variable);
            if (var_index == -1) {
                printf("Unknown variable '%s'!\n", variable);
                return 10;
            }

            general_vars->used_vars[var_index] = 1;

            /* Populate the simplex tableau row */
            arr[var_index] = coefficient;
        } else {
            return 93;
        }

        token = strtok(NULL, "+");
    }

    return 0;
}