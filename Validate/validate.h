#ifndef __VALIDATE__
#define __VALIDATE__

#define MAX_LINE_LENGTH 256
#define MAX_VARIABLES 100
#define MAX_SECTIONS 6
#define MAX_VAR_NAME 50

#include "../Generals/generals.h"

int is_var_known(const General_vars *general_vars, const char *var_name);
int is_valid_string(const char *str);
int is_valid_operator_char(char c);
int contains_only_valid_operators(const char *str);
int is_valid_operator(const char *str);
int contains_invalid_operator_sequence(char *str);
int bounds_valid_operators(const char *str);
int check_unused_variables(General_vars *general_vars);
int check_invalid_chars(char *line, char *invalid_chars);

#endif
