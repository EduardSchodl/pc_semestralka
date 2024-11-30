#ifndef __OBJECTIVES__
#define __OBJECTIVES__

#include "../LProblem/lp.h"

int parse_objectives(char **expressions, SimplexTableau *tableau, General_vars *general_vars, double objective_row[], int num_lines);

#endif
