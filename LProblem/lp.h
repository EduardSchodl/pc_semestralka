#ifndef __LP__
#define __LP__
#include "../Generals/generals.h"
#include "../Bounds/bounds.h"

typedef struct {
    char *type;
    double **tableau;
    int col_count;
    int row_count;
} SimplexTableau;

int simplex(SimplexTableau *tableau, double objective_row[], General_vars *general_vars, Bounds *bounds);
int find_pivot_col(const SimplexTableau *tableau, int minimization);
int find_pivot_row(const SimplexTableau *tableau, const int col_index);
SimplexTableau *create_simplex_tableau(int num_constraints, int num_variables);
int check_solution_bounds(SimplexTableau *tableau, General_vars *general_vars, Bounds *bounds);
void print_solution(const SimplexTableau *tableau, const General_vars *general_vars);
void free_simplex_tableau(SimplexTableau *tableau);

void print_tableau(SimplexTableau *simplex_tableau);

int simplex_phase_one(SimplexTableau *tableau);
int simplex_phase_two(SimplexTableau *tableau, int num_general_vars);
int remove_artificial_variables(SimplexTableau *tableau, int num_artificial_vars);

int has_nonzero_in_objective_row(SimplexTableau *tableau, int num_general_vars);
double my_fabs(double x);

#endif