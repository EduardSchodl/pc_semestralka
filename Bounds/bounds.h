#ifndef __BOUNDS__
#define __BOUNDS__

#ifndef INFINITY
#define INFINITY (1.0 / 0.0)
#define NEGATIVE_INFINITY (-INFINITY)
#endif

typedef struct {
    char **var_names;
    double *lower_bound;
    double *upper_bound;
    int num_vars;
    int max_vars;
} Bounds;

Bounds *create_bounds(const int initial_size);
void free_bounds(Bounds *bounds);
void add_bound(Bounds *bounds, const char *var_name, const double lower_bound, const double upper_bound);
int parse_bounds(Bounds *bounds, char *line);

int is_number(char *str);
int is_operator(char *token);

#endif
