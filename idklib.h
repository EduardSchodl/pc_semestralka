#ifndef __LIB__
#define __LIB__

#define MAX_VARS 100
#define MAX_CONSTRAINTS 100
#define LP_EXT ".lp"
#define OUTPUT_FILE_EXT ".txt"
#define INITIAL_SIZE 2

#ifndef INFINITY
#define INFINITY (1.0 / 0.0)
#define NEGATIVE_INFINITY (-INFINITY)
#endif

typedef struct {
    char *type;
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
    char *var_name;
    double lower_bound;
    double upper_bound;
} Bounds;

typedef struct {
    Bounds **bounds_array;
    int capacity;
    int count;
} BoundsList;

typedef struct {
    char **general_vars;
    int num_general_vars;
    Bounds **bounds;
    int max_vars;
} GeneralVars;

BoundsList* create_bounds_list(int initial_capacity);
void add_bound(BoundsList *bounds_list, Bounds *bound);
void free_bounds_list(BoundsList *bounds_list);
void bind_bounds(const GeneralVars *general_vars, const BoundsList *bounds_list);
char *trim_white_space(char *str);
void remove_spaces(char *str);
void parse_constraints(char *line);

void read_input_file();
GeneralVars* create_general_vars(int initial_size);
void add_variable(GeneralVars* gv, const char *var_name);
Bounds *parse_bounds(char *line);
void free_general_vars(GeneralVars *gv);

#endif