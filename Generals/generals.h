#ifndef __GENERALS__
#define __GENERALS__

typedef struct {
    char **general_vars;
    int num_general_vars;
    int max_vars;
} General_vars;

General_vars* create_general_vars(const int initial_size);
int parse_generals(General_vars *general_vars, const char *line);
void add_variable(General_vars *gv, const char *var_name);
void free_general_vars(General_vars *general_vars);
int get_var_index(General_vars *general_vars, char *var_name);

#endif
