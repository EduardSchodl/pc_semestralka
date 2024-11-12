#ifndef __LP__
#define __LP__
typedef struct {
    double **data;
    int col_count;
    int row_count;
} LP;

void simplex(double **tableau, int numConstraints, int numVariables);
int find_pivot_col(const LP *tableau);
int find_pivot_row(const LP *tableau, const int col_index);
LP *create_tableau(const int rows, const int cols);
int check_bounds(const LP *tableau);
void print_solution(const LP *tableau);

#endif
