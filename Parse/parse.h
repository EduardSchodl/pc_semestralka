#ifndef __PARSE__
#define __PARSE__

#define INITIAL_SIZE 2

#include "../File/file.h"
#include "../LProblem/lp.h"
#include "../Generals/generals.h"

typedef struct {
    double coefficient;
    char variable[10];
} Term;

char *trim_white_space(char *str);
char *remove_spaces(char *str);

int parse_subject_to(char **expression, int num_of_constraints, SimplexTableau *tableau, General_vars *general_vars);

int parse_objectives(char **expressions, SimplexTableau *tableau, General_vars *general_vars, double objective_row[], int num_lines);
int extract_variable_and_coefficient(char *segment, char *variable, double *coefficient);
double parse_coefficient(const char *token);

SectionBuffers* create_section_buffers(int initial_size);
void free_section_buffers(SectionBuffers *buffers);
void add_line_to_buffer(char ***buffer, int *count, char *line);

void normalize_expression(char *expression);
int simplify_expression(const char *expression, char *simplified_expression);
void add_term(Term terms[], int *term_count, double coefficient, const char *variable);
int check_matching_parentheses(const char *expression);

#endif
