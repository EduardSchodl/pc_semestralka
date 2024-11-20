#ifndef __PARSE__
#define __PARSE__

#define INITIAL_SIZE 2

#include "file.h"
#include "lp.h"
#include "Generals/generals.h"

typedef struct {
    int coefficient;
    char variable[10];
} Term;

char *trim_white_space(char *str);
char *remove_spaces(char *str);
int parse_lines(SectionBuffers *buffers, SimplexTableau *tableau, General_vars **general_vars);
int pre_parse(SectionBuffers *section_buffers, int *var_num, int *subject_to_count);

int parse_subject_to(char **expression, int len, SimplexTableau *tableau, General_vars *general_vars);

int parse_objectives(char *expression, SimplexTableau *tableau, General_vars *general_vars);
int extract_variable_and_coefficient(char *segment, char *variable, double *coefficient);
double parse_coefficient(const char *token);

SectionBuffers* create_section_buffers(int initial_size);
void free_section_buffers(SectionBuffers *buffers);
void add_line_to_buffer(char ***buffer, int *count, char *line);

void normalize_expression(char *expression);
void simplify_expression(const char *expression, char *simplified_expression);
void add_term(Term terms[], int *term_count, int coefficient, const char *variable);

#endif
