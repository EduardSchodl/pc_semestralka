#ifndef __PARSE__
#define __PARSE__

#define INITIAL_SIZE 2

#include "Generals/generals.h"

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

char *trim_white_space(char *str);
char *remove_spaces(char *str);
int process_lines(char **lines);
int parse_lines(SectionBuffers *buffers);

int parse_subject_to(char *line, Matrix *matrix, General_vars *general_vars);

int parse_objectives(char *expression, Matrix *matrix, General_vars *general_vars);

SectionBuffers* create_section_buffers(int initial_size);
void free_section_buffers(SectionBuffers *buffers);
void add_line_to_buffer(char ***buffer, int *count, char *line);

#endif
