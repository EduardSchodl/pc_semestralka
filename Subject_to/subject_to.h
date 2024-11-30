#ifndef __SUBJECTTO__
#define __SUBJECTTO__

#include "../LProblem/lp.h"

int parse_subject_to(char **expression, int num_of_constraints, SimplexTableau *tableau, General_vars *general_vars);

#endif
