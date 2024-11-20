#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include "lp.h"

#include "file.h"
#include "Generals/generals.h"

void simplex(SimplexTableau *tableau, int minimization) {
    int most_negative_col;
    int smallest_quotient_row;
    double pivot, factor;
    int i, j;

    printf("Minimize: %d\n", minimization);

    while (1) {
        /* Adjust pivot column selection based on the minimization flag */
        most_negative_col = find_pivot_col(tableau, minimization);

        /* Optimal solution found */
        if (most_negative_col == -1) {
            break;
        }

        print_tableau(tableau);

        smallest_quotient_row = find_pivot_row(tableau, most_negative_col);
        if (smallest_quotient_row == -1) {
            printf("Objective function is unbounded.\n");
            return;
        }

        pivot = tableau->tableau[smallest_quotient_row][most_negative_col];

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

int check_bounds(const SimplexTableau *tableau) {
    int i;
    double value;

    for (i = 0; i < tableau->row_count - 1; i++) {
        value = tableau->tableau[i][tableau->col_count - 1];
        if (value < 0) {
            return 0;
        }
    }

    return 1;
}

SimplexTableau *create_simplex_tableau(int num_constraints, int num_variables) {
    SimplexTableau *temp;
    int i, j;
    int num_slack_vars = num_constraints;
    int num_cols = num_variables + num_slack_vars + 1;
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
    double best_value = 0;
    double current_value;

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
