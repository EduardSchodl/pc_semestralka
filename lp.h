#ifndef __LP__
#define __LP__
typedef struct {
    double **data;
    int col_count;
    int row_count;
} LP;

void simplex(double **tableau, int numConstraints, int numVariables);
int find_pivot_col(LP *tableau);
int find_pivot_row(LP *tablaeu, int col_index);
LP *create_tableau();
#endif
