#ifndef __PARSE__
#define __PARSE__

#define MAX_VARS 100
#define INITIAL_SIZE 2

#ifndef INFINITY
#define INFINITY (1.0 / 0.0)
#define NEGATIVE_INFINITY (-INFINITY)
#endif

typedef enum {
    TOKEN_NUMBER,
    TOKEN_VARIABLE,
    TOKEN_OPERATOR
} TokenType;

typedef struct {
    TokenType type;
    char value[64];
} Token;

typedef struct {
    char *type;
    char **var_names;
    double *coefficients;
    int num_vars;
} Objectives;

typedef struct {
    char name[50];
    double *lhs;
    char *relation;
    double rhs;
} Constraints;

typedef struct {
    char **var_names;
    double *lower_bound;
    double *upper_bound;
    int num_vars;
    int max_vars;
} Bounds;

typedef struct {
    char **general_vars;
    int num_general_vars;
    int max_vars;
} General_vars;

int process_lines(char **lines);
void add_bound(Bounds *bounds, const char *var_name, const double lower_bound, const double upper_bound);
char *trim_white_space(char *str);
char *remove_spaces(char *str);

void parse_constraints(Constraints *constraints, char *line);
void parse_objectives(Objectives *objectives, char *line);
int parse_bounds(Bounds *bounds, char *line);
int parse_generals(General_vars *general_vars, char *line);

General_vars* create_general_vars(int initial_size);
Objectives *create_objectives(int initial_size);
Constraints *create_constraints(int initial_size);
Bounds *create_bounds(int initial_size);
void add_variable(General_vars* gv, const char *var_name);

int allocate_structs(General_vars *general_vars, Objectives *objectives, Constraints *constraints, Bounds *bounds);
void free_structures(General_vars *general_vars, Objectives *objectives, Constraints *constraints, Bounds *bounds);
void free_objectives(Objectives *objectives);
void free_constraints(Constraints *constraints);
void free_bounds(Bounds *bounds);

void free_general_vars(General_vars *gv);
int tokenize(const char *expr, Token *tokens, int max_tokens);
void shunting_yard(const char *infix, char *postfix);
int precedence(char op);

#endif
