#ifndef __LP__
#define __LP__
#include "Generals/generals.h"

typedef struct {
    char *type;
    double **tableau;
    int col_count;
    int row_count;
} SimplexTableau;

void simplex(SimplexTableau *tableau);
int find_pivot_col(const SimplexTableau *tableau);
int find_pivot_row(const SimplexTableau *tableau, const int col_index);
SimplexTableau *create_simplex_tableau(int num_constraints, int num_variables);
int check_bounds(const SimplexTableau *tableau);
void print_solution(const SimplexTableau *tableau, const General_vars *general_vars);
void free_simplex_tableau(SimplexTableau *tableau);

#endif
