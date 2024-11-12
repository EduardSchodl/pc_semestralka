#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include "lp.h"

void simplex(LP *tableau) {
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

        pivot = tableau->data[smallest_quotient_row][most_negative_col];

        for (j = 0; j < tableau->col_count; j++) {
            tableau->data[smallest_quotient_row][j] /= pivot;
        }

        for (i = 0; i < tableau->row_count; i++) {
            if (i != smallest_quotient_row) {
                factor = tableau->data[i][most_negative_col];
                for (j = 0; j < tableau->col_count; j++) {
                    tableau->data[i][j] -= factor * tableau->data[smallest_quotient_row][j];
                }
            }
        }
    }
}

int check_bounds(const LP *tableau) {
    int i;
    double value;

    for (i = 0; i < tableau->row_count - 1; i++) {
        value = tableau->data[i][tableau->col_count - 1];
        if (value < 0) {
            return 0;
        }
    }

    return 1;
}

LP *create_tableau(const int rows, const int cols) {
    LP *temp;
    int i;

    temp = (LP *)malloc(sizeof(LP));
    if(!temp) {
        return NULL;
    }

    temp->row_count = rows;
    temp->col_count = cols;

    temp->data = (double **)malloc(rows * sizeof(double *));
    if(!temp->data) {
        free(temp);
        return NULL;
    }

    for(i = 0; i < cols; i++) {
        temp->data[i] = (double *)calloc(cols, sizeof(double));
        if(!temp->data[i]) {
            for(i--; i >= 0; i--) {
                free(temp->data[i]);
            }
            free(temp->data);
            free(temp);
            return NULL;
        }
    }

    return temp;
}

void print_solution(const LP *tableau) {
    printf("Optimal solution:\n");
    for (int i = 0; i < tableau->row_count - 1; i++) {
        printf("Variable %d = %lf\n", i + 1, tableau->data[i][tableau->col_count - 1]);
    }
    printf("Optimal value: %lf\n", tableau->data[tableau->row_count - 1][tableau->col_count - 1]);
}

int find_pivot_row(const LP *tableau, const int col_index) {
    int i;
    int smallest_quotient_row = -1;
    double smallest_ratio = DBL_MAX;
    double ratio;
    double element;

    for (i = 0; i < tableau->row_count - 1; i++) {
        element = tableau->data[i][col_index];
        if (element > 0) {
            ratio = tableau->data[i][tableau->col_count - 1] / element;
            if (ratio < smallest_ratio) {
                smallest_ratio = ratio;
                smallest_quotient_row = i;
            }
        }
    }
    return smallest_quotient_row;
}

int find_pivot_col(const LP *tableau) {
    int i;
    int most_negative_col = -1;
    double most_negative_value = 0;

    for (i = 0; i < tableau->col_count - 1; i++) {
        if (tableau->data[tableau->row_count - 1][i] < most_negative_value) {
            most_negative_value = tableau->data[tableau->row_count - 1][i];
            most_negative_col = i;
        }
    }

    return most_negative_col;
}