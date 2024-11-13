#ifndef __PARSE__
#define __PARSE__

#define INITIAL_SIZE 2

#ifndef INFINITY
#define INFINITY (1.0 / 0.0)
#define NEGATIVE_INFINITY (-INFINITY)
#endif

typedef struct {
    char **general_lines;
    int general_count;
    char **subject_to_lines;
    int subject_to_count;
    char **objective_lines;
    int objective_count;
    char **bounds_lines;
    int bounds_count;
} SectionBuffers;

typedef struct {
    char *type;
    double *arr_A;
    double *arr_B;
    double *objectives_row;
} Matrix;

typedef struct {
    char **general_vars;
    int num_general_vars;
    int max_vars;
} General_vars;

typedef struct {
    char **var_names;
    double *lower_bound;
    double *upper_bound;
    int num_vars;
    int max_vars;
} Bounds;

char *trim_white_space(char *str);
int process_lines(char **lines);
int parse_lines(SectionBuffers *buffers);

General_vars* create_general_vars(const int initial_size);
int parse_generals(General_vars *general_vars, char *line);
void add_variable(General_vars *gv, const char *var_name);
void free_general_vars(General_vars *general_vars);

Bounds *create_bounds(const int initial_size);
void free_bounds(Bounds *bounds);
void add_bound(Bounds *bounds, const char *var_name, const double lower_bound, const double upper_bound);
int parse_bounds(Bounds *bounds, char *line);

int parse_subject_to(char *line, Matrix *matrix, General_vars *general_vars);

int parse_objectives(char *expression, Matrix *matrix, General_vars *general_vars);

SectionBuffers* create_section_buffers(int initial_size);
void free_section_buffers(SectionBuffers *buffers);
void add_line_to_buffer(char ***buffer, int *count, char *line);

#endif
