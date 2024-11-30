#ifndef __PARSE__
#define __PARSE__

#define INITIAL_SIZE 2

typedef struct {
    double coefficient;
    char variable[10];
} Term;

char *trim_white_space(char *str);
char *remove_spaces(char *str);
int extract_variable_and_coefficient(char *segment, char *variable, double *coefficient);
double parse_coefficient(const char *token);
void normalize_expression(char *expression);
int simplify_expression(const char *expression, char *simplified_expression);
void add_term(Term terms[], int *term_count, double coefficient, const char *variable);
int check_matching_parentheses(const char *expression);

#endif
