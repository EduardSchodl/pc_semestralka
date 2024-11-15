#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include "lp.h"

void simplex(SimplexTableau *tableau) {
    int most_negative_col;
    int smallest_quotient_row;
    double pivot, factor;
    int i, j;

    while(1) {
        most_negative_col = find_pivot_col(tableau);

        /* optimal solution found */
        if (most_negative_col == -1) {
            break;
        }

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

    temp = (SimplexTableau *)malloc(sizeof(SimplexTableau));
    if(!temp) {
        return NULL;
    }

    temp->row_count = num_rows;
    temp->col_count = num_cols;

    temp->tableau = (double **)malloc(num_rows * sizeof(double *));
    if(!temp->tableau) {
        free(temp);
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

void print_solution(const SimplexTableau *tableau) {
    int i;

    printf("Optimal solution:\n");
    for (i = 0; i < tableau->row_count - 1; i++) {
        printf("Variable %d = %0.6f\n", i + 1, tableau->tableau[i][tableau->col_count - 1]);
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

int find_pivot_col(const SimplexTableau *tableau) {
    int i;
    int most_negative_col = -1;
    double most_negative_value = 0;

    for (i = 0; i < tableau->col_count - 1; i++) {
        if (tableau->tableau[tableau->row_count - 1][i] < most_negative_value) {
            most_negative_value = tableau->tableau[tableau->row_count - 1][i];
            most_negative_col = i;
        }
    }

    return most_negative_col;
}