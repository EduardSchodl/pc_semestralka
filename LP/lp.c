#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include "lp.h"

#include <string.h>

#include "../File/file.h"
#include "../Generals/generals.h"

int simplex(SimplexTableau *tableau, double objective_row[], General_vars *general_vars) {
    int i;
    int num_artificial_vars;

    if(!tableau || !objective_row || !general_vars) {
        return 93;
    }

    num_artificial_vars = tableau->row_count - 1;

    print_tableau(tableau);
    if (simplex_phase_one(tableau) != 0) {
        printf("No feasible solution exists.\n" );
        return 21;
    }

    remove_artificial_variables(tableau, num_artificial_vars);

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

    print_solution(tableau, general_vars);

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
        new_row = (double *) malloc(new_col_count * sizeof(double));
        if (!new_row) {
            printf("Memory allocation error during artificial variable removal.\n");
            return 1;
        }

        for (j = 0, k = 0; j < tableau->col_count; j++) {
            if (j < tableau->col_count - num_artificial_vars - 1 || j == tableau->col_count - 1) {
                new_row[k++] = tableau->tableau[i][j];
            }
        }

        free(tableau->tableau[i]);
        tableau->tableau[i] = new_row;
    }

    tableau->col_count = new_col_count;

    return 0;
}

int check_bounds(const SimplexTableau *tableau) {
    int i;
    double value;

    if(!tableau) {
        return 1;
    }

    for (i = 0; i < tableau->row_count - 1; i++) {
        value = tableau->tableau[i][tableau->col_count - 1];
        if (value < 0) {
            return 1;
        }
    }

    return 0;
}

SimplexTableau *create_simplex_tableau(int num_constraints, int num_variables) {
    SimplexTableau *temp;
    int i, j;
    int num_slacks = num_constraints;
    int num_artificials = num_constraints;
    int num_cols = num_variables + num_slacks + num_artificials + 1;
    int num_rows = num_constraints + 1;

    temp = (SimplexTableau *) malloc(sizeof(SimplexTableau));
    if (!temp) {
        return NULL;
    }

    temp->row_count = num_rows;
    temp->col_count = num_cols;

    temp->tableau = (double **) malloc(num_rows * sizeof(double *));
    if (!temp->tableau) {
        free(temp);
        return NULL;
    }

    temp->type = malloc(LINE_MAX_SIZE * sizeof(char *));
    if (!temp->type) {
        printf("Memory allocation failed for tableau->type\n");
        return NULL;
    }

    for (i = 0; i < num_rows; i++) {
        temp->tableau[i] = calloc(num_cols, sizeof(double));
        if (!temp->tableau[i]) {
            for (j = 0; j < i; j++) {
                free(temp->tableau[j]);
            }
            free(temp->tableau);
            free(temp);
            return NULL;
        }
    }

    return temp;
}

void print_solution(const SimplexTableau *tableau, const General_vars *general_vars) {
    int i;

    if (!tableau || !general_vars) {
        return;
    }

    printf("Optimal solution:\n");
    for (i = 0; i < tableau->row_count - 1; i++) {
        printf("%s = %0.6f\n", general_vars->general_vars[i], tableau->tableau[i][tableau->col_count - 1]);
    }
    printf("Optimal value: %0.6f\n", tableau->tableau[tableau->row_count - 1][tableau->col_count - 1]);
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

    return pivot_col;
}

void free_simplex_tableau(SimplexTableau *tableau) {
    int i;

    if (!tableau) {
        return;
    }

    if (tableau->type) {
        free(tableau->type);
    }

    if (tableau->tableau) {
        for (i = 0; i < tableau->row_count; i++) {
            if (tableau->tableau[i]) {
                free(tableau->tableau[i]);
            }
        }
        free(tableau->tableau);
    }

    free(tableau);
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

/* minimalizace artificial variables */
int simplex_phase_one(SimplexTableau *tableau) {
    int most_negative_col;
    int smallest_quotient_row;
    double pivot, factor;
    int i, j;
    int all_non_negative;

    if(!tableau) {
        return 1;
    }

    while (1) {
        all_non_negative = 1;
        if(tableau->tableau[tableau->row_count-1][tableau->col_count-1] == 0) {
            printf("idk man\n");
            return 0;
        }

        for (i = 0; i < tableau->col_count - 1; i++) {
            if (tableau->tableau[tableau->row_count - 1][i] < -1e-6) {
                all_non_negative = 0;
                break;
            }
        }

        if (all_non_negative) {
            if (tableau->tableau[tableau->row_count - 1][tableau->col_count - 1] > 1e-6) {
                printf("The problem is infeasible.\n");
                return 1;
            }
            printf("Phase One complete. Feasible solution found.\n");
            return 0;
        }

        most_negative_col = find_pivot_col(tableau, 1);
        if (most_negative_col == -1) {
            printf("Error: No pivot column found.\n");
            return 1;
        }

        smallest_quotient_row = find_pivot_row(tableau, most_negative_col);
        if (smallest_quotient_row == -1) {
            printf("Objective function is unbounded or numerically unstable.\n");
            return 1;
        }

        pivot = tableau->tableau[smallest_quotient_row][most_negative_col];
        printf("pivot = %f, coords: %d,%d\n", pivot, smallest_quotient_row, most_negative_col);
        for (j = 0; j < tableau->col_count; j++) {
            tableau->tableau[smallest_quotient_row][j] /= pivot;
        }
        for (i = 0; i < tableau->row_count; i++) {
            if (i != smallest_quotient_row) {
                factor = tableau->tableau[i][most_negative_col];
                for (j = 0; j < tableau->col_count; j++) {
                    tableau->tableau[i][j] -= factor * tableau->tableau[smallest_quotient_row][j];
                }
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
    int most_negative_col;
    int smallest_quotient_row;
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
                printf("Non-zero value found in the objective row for general variables. Continuing pivoting.\n");
                most_negative_col = find_pivot_col(tableau, 0);
                if (most_negative_col == -1) {
                    printf("Error: No pivot column found.\n");
                    return 1;
                }
            } else {
                printf("Phase Two complete. Feasible solution found.\n");
                return 0;
            }
        }


        most_negative_col = find_pivot_col(tableau, 0);
        if (most_negative_col == -1) {
            printf("The problem is unbounded.\n");
            return 1;
        }

        smallest_quotient_row = find_pivot_row(tableau, most_negative_col);
        if (smallest_quotient_row == -1) {
            printf("The problem is unbounded.\n");
            return 1;
        }

        pivot = tableau->tableau[smallest_quotient_row][most_negative_col];
        printf("val: %f, coord: %d, %d\n", pivot, smallest_quotient_row, most_negative_col);
        for (j = 0; j < tableau->col_count; j++) {
            tableau->tableau[smallest_quotient_row][j] /= pivot;
        }

        for (i = 0; i < tableau->row_count; i++) {
            if (i != smallest_quotient_row) {
                factor = tableau->tableau[i][most_negative_col];
                for (j = 0; j < tableau->col_count; j++) {
                    tableau->tableau[i][j] -= factor * tableau->tableau[smallest_quotient_row][j];
                }
            }
        }

        print_tableau(tableau);
    }
}

