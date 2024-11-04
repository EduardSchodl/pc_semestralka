#ifndef __LIB__
#define __LIB__

#define MAX_VARS 100
#define MAX_CONSTRAINTS 100

#ifndef INFINITY
#define INFINITY (1.0 / 0.0)
#define NEGATIVE_INFINITY (-INFINITY)
#endif

typedef struct {
    char type[10];
    double coefficients[MAX_VARS];
    int num_vars;
} Objective;

typedef struct {
    char name[50];
    double lhs[MAX_VARS];
    char relation;
    double rhs;
} Constraint;

typedef struct {
    double lower_bound;
    double upper_bound;
} Bounds;

typedef struct {
    char **general_vars;
    int num_general_vars;
    Bounds *bounds;
    int max_vars;
} GeneralVars;

void read_input_file();
GeneralVars* create_general_vars(int initial_size);
void add_variable(GeneralVars* gv, char *var);
void parse_bounds(char *line);
void free_general_vars(GeneralVars *gv);
void solve_linear_programming();
void print_solution();

#endif