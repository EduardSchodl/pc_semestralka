#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Validate/validate.h"
#include "parse.h"
#include "../File/file.h"
#include "../LProblem/lp.h"
#include "../Generals/generals.h"

char* trim_white_space(char* str) {
    char *end;

    if (!str) {
        return NULL;
    }

    while (*str && isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0') return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }

    *(end + 1) = '\0';

    return str;
}


char *remove_spaces(char *str){
    int count = 0, i;

    if (!str) {
        return NULL;
    }

    for (i = 0; str[i]; i++) {
        if (str[i] != ' ') {
            str[count++] = str[i];
        }
    }

    str[count] = '\0';

    return str;
}

SectionBuffers* create_section_buffers(int initial_size) {
    SectionBuffers *buffers = NULL;

    if (!initial_size) {
        return NULL;
    }

    buffers = (SectionBuffers *)malloc(sizeof(SectionBuffers));
    if (!buffers) {
        return NULL;
    }

    buffers->general_lines = malloc(initial_size * sizeof(char *));
    if (!buffers->general_lines) {
        free_section_buffers(buffers);
        return NULL;
    }

    buffers->subject_to_lines = malloc(initial_size * sizeof(char *));
    if (!buffers->subject_to_lines) {
        free_section_buffers(buffers);
        return NULL;
    }

    buffers->objective_lines = malloc(initial_size * sizeof(char *));
    if (!buffers->objective_lines) {
        free_section_buffers(buffers);
        return NULL;
    }

    buffers->bounds_lines = malloc(initial_size * sizeof(char *));
    if (!buffers->bounds_lines) {
        free_section_buffers(buffers);
        return NULL;
    }

    buffers->general_count = 0;
    buffers->subject_to_count = 0;
    buffers->objective_count = 0;
    buffers->bounds_count = 0;

    return buffers;
}

void free_section_buffers(SectionBuffers *buffers) {
    int i;

    if (!buffers) {
        return;
    }

    if (buffers->general_lines) {
        for (i = 0; i < buffers->general_count; i++) {
            /* free(&buffers->general_lines[i]); */
            free(buffers->general_lines[i]);
        }
        free(buffers->general_lines);
        buffers->general_lines = NULL;
    }

    if (buffers->subject_to_lines) {
        for (i = 0; i < buffers->subject_to_count; i++) {
            free(buffers->subject_to_lines[i]);
        }
        free(buffers->subject_to_lines);
        buffers->subject_to_lines = NULL;
    }

    if (buffers->objective_lines) {
        for (i = 0; i < buffers->objective_count; i++) {
            free(buffers->objective_lines[i]);
        }
        free(buffers->objective_lines);
        buffers->objective_lines = NULL;
    }

    if (buffers->bounds_lines) {
        for (i = 0; i < buffers->bounds_count; i++) {
            free(buffers->bounds_lines[i]);
        }
        free(buffers->bounds_lines);
        buffers->bounds_lines = NULL;
    }

    free(buffers);
    buffers = NULL;
}


void add_line_to_buffer(char ***buffer, int *count, char *line) {
    char **temp;

    if (!buffer || !*buffer || !line) {
        return;
    }

    temp = realloc(*buffer, (*count + 1) * sizeof(char *));
    if (!temp) {
        printf("Memory reallocation failed.\n");
        return;
    }

    *buffer = temp;
    trim_white_space(line);

    (*buffer)[*count] = malloc(strlen(line) + 1);
    if (!(*buffer)[*count]) {
        printf("Memory allocation for line failed.\n");
        return;
    }

    strncpy((*buffer)[*count], line, strlen(line) + 1);

    (*count)++;
}



double parse_coefficient(const char *token) {
    char *end_ptr;
    double coeff = strtod(token, &end_ptr);

    if(!token) {
        return 0;
    }

    if (end_ptr == token) {
        return (token[0] == '-') ? -1.0 : 1.0;
    }

    return coeff;
}

int extract_variable_and_coefficient(char *segment, char *variable, double *coefficient) {
    char *ptr = segment;
    char coeff_buffer[32] = {0};
    int i = 0, index;
    char *e;

    if (!segment || !variable) {
        return 1;
    }

    e = strchr(ptr, '*');
    if (e) {
        index = (int)(e - ptr);
        memmove(&ptr[index], &ptr[index + 1], strlen(ptr) - index);
    }

    while (*ptr && (isdigit(*ptr) || *ptr == '.' || *ptr == '-')) {
        coeff_buffer[i++] = *ptr;
        ptr++;
    }

    if (i > 0) {
        *coefficient = parse_coefficient(coeff_buffer);
    } else {
        *coefficient = 1.0;
    }

    if (*ptr == '*') {
        ptr++;
    }

    strcpy(variable, ptr);

    /*printf("Segment: %s | Variable: %s | Coefficient: %f\n", segment, variable, *coefficient);*/
    return is_valid_string(variable) ? 1 : 0;
}


int parse_objectives(char **expressions, SimplexTableau *tableau, General_vars *general_vars, double objective_row[], int num_lines) {
    char *token;
    char variable[64];
    double coefficient;
    char modified_expression[256];
    char simplified_expression[256];
    char expression[256];
    int j = 0, i, var_index;
    int res_code;

    if (!expressions || !*expressions || !tableau || !general_vars || !objective_row) {
        return 93;
    }

    for (i = 0; i < num_lines; i++) {
        if (i == 0) {
            strcpy(tableau->type, expressions[i]);
            continue;
        }

        strcpy(expression, expressions[i]);

        remove_spaces(expression);

        normalize_expression(expression);

        if((res_code = simplify_expression(expression, simplified_expression))) {
            return res_code;
        }

        for (i = 0; simplified_expression[i] != '\0'; i++) {
            if (simplified_expression[i] == '-') {
                if (i > 0 && simplified_expression[i - 1] != '+' && simplified_expression[i - 1] != '-') {
                    modified_expression[j++] = '+';
                }
            }
            modified_expression[j++] = simplified_expression[i];
        }
        modified_expression[j] = '\0';

        token = strtok(modified_expression, "+");
        while (token != NULL) {
            remove_spaces(token);
            if (!extract_variable_and_coefficient(token, variable, &coefficient)) {
                var_index = get_var_index(general_vars, variable);
                if (var_index == -1) {
                    printf("Unknown variable '%s'!\n", variable);
                    return 10;
                }

                general_vars->used_vars[var_index] = 1;

                objective_row[var_index] = coefficient;


                /*printf("Variable '%s' at index %d with coefficient %f\n", variable, var_index, coefficient);*/
            } else {
                return 93;
            }

            token = strtok(NULL, "+");
        }
    }

    return 0;
}

void normalize_expression(char *expression) {
    int i;

    if(!expression) {
        return;
    }

    for(i = 0; expression[i]; i++) {
        if(expression[i] == '{' || expression[i] == '[') {
            expression[i] = '(';
        }
        if(expression[i] == '}' || expression[i] == ']') {
            expression[i] = ')';
        }
    }
}

int check_matching_parentheses(const char *expression) {
    int stack = 0;
    int i;

    for (i = 0; expression[i] != '\0'; i++) {
        if (expression[i] == '(') {
            stack++;
        } else if (expression[i] == ')') {
            stack--;
            if (stack < 0) {
                return 1;
            }
        }
    }

    return stack == 0 ? 0 : 1;
}

void add_term(Term terms[], int *term_count, double coefficient, const char *variable) {
    int i;

    if (!terms || !variable) {
        return;
    }

    for (i = 0; i < *term_count; i++) {
        if (strcmp(terms[i].variable, variable) == 0) {
            terms[i].coefficient += coefficient;
            return;
        }
    }
    terms[*term_count].coefficient = coefficient;
    strcpy(terms[*term_count].variable, variable);
    (*term_count)++;
}

void process_term(Term terms[], int *term_count, double coefficient, int sign, char *variable) {
    if(!terms || !variable) {
        return;
    }

    if (variable[0] != '\0') {
        add_term(terms, term_count, coefficient * sign, variable);
        variable[0] = '\0';
    }
}

int simplify_expression(const char *expression, char *simplified_expression) {
    Term terms[100];
    int term_count = 0;
    double coefficient = 0;
    int sign = 1;
    int neg_stack[100];
    int stack_top = 0;
    char variable[50];
    int i, j;
    char c;
    int reading_coefficient = 0;
    int reading_decimal = 0;
    double decimal_divisor = 1.0;
    double multiplier_stack[100];
    double current_multiplier = 1.0;
    char buffer[256];
    int buffer_len = 0;
    int idx;

    if (!expression || !simplified_expression) {
        return 93;
    }

    if (!check_matching_parentheses(expression)) {
        printf("Syntax error!\n");
        return 11;
    }

    stack_top = 0;
    neg_stack[stack_top++] = 1;
    multiplier_stack[stack_top - 1] = 1.0;

    for (i = 0; expression[i] != '\0'; i++) {
        c = expression[i];

        if (isdigit(c)) {
            if (!reading_coefficient) {
                coefficient = 0;
                reading_coefficient = 1;
                reading_decimal = 0;
                decimal_divisor = 1.0;
            }
            if (reading_decimal) {
                decimal_divisor *= 10;
                coefficient += (c - '0') / decimal_divisor;
            } else {
                coefficient = coefficient * 10 + (c - '0');
            }
        } else if (c == '.') {
            reading_decimal = 1;
        } else if (c == '(') {
            if (!reading_coefficient) coefficient = 1;
            current_multiplier *= coefficient * sign * neg_stack[stack_top - 1];
            multiplier_stack[stack_top] = current_multiplier;
            neg_stack[stack_top] = neg_stack[stack_top - 1];
            stack_top++;
            coefficient = 0;
            reading_coefficient = 0;
            sign = 1;
        } else if (c == ')') {
            stack_top--;
            current_multiplier = multiplier_stack[stack_top];
        } else if (isalpha(c) || c == '_') {
            idx = 0;
            while (isalnum(c) || c == '_') {
                variable[idx++] = c;
                c = expression[++i];
            }
            variable[idx] = '\0';
            i--;
            if (!reading_coefficient) coefficient = 1;
            process_term(terms, &term_count, coefficient * current_multiplier, sign * neg_stack[stack_top - 1], variable);
            coefficient = 0;
            reading_coefficient = 0;
        } else if (c == '*') {
            continue;
        } else if (c == '+') {
            if (reading_coefficient) {
                process_term(terms, &term_count, coefficient * current_multiplier, sign * neg_stack[stack_top - 1], "");
            }
            coefficient = 0;
            sign = 1;
            reading_coefficient = 0;
        } else if (c == '-') {
            if (reading_coefficient) {
                process_term(terms, &term_count, coefficient * current_multiplier, sign * neg_stack[stack_top - 1], "");
            }
            coefficient = 0;
            sign = -1;
            reading_coefficient = 0;
        }
    }

    if (reading_coefficient) {
        process_term(terms, &term_count, coefficient * current_multiplier, sign * neg_stack[stack_top - 1], "");
    }

    buffer_len = 0;
    for (i = 0; i < term_count; i++) {
        if (terms[i].coefficient != 0) {
            if (buffer_len > 0 && terms[i].coefficient > 0) {
                buffer_len += snprintf(buffer + buffer_len, 256 - buffer_len, "+");
            }
            if (terms[i].coefficient == -1 && terms[i].variable[0] != '\0') {
                buffer_len += snprintf(buffer + buffer_len, 256 - buffer_len, "-");
            } else if (terms[i].coefficient != 1 || terms[i].variable[0] == '\0') {
                buffer_len += snprintf(buffer + buffer_len, 256 - buffer_len, "%f", terms[i].coefficient);
            }
            if (terms[i].variable[0] != '\0') {
                buffer_len += snprintf(buffer + buffer_len, 256 - buffer_len, "%s", terms[i].variable);
            }
        }
    }
    strcpy(simplified_expression, buffer);

    return 0;
}


int parse_subject_to(char **expressions, int num_of_constraints, SimplexTableau *tableau, General_vars *general_vars) {
    char *left_side;
    char *right_side;
    char *delim;
    char *delim_pos;
    char *name_pos;
    char *token;
    char variable[64];
    double coefficient;
    char modified_expression[256];
    char expression_copy[512];
    char simplified_expression[256];
    int j, i, var_index, a;
    int res_code = 0;

    if(!expressions || !*expressions || !tableau || !general_vars) {
        return 93;
    }

    for (a = 0; a < num_of_constraints; a++) {
        memset(modified_expression, 0, sizeof(modified_expression));
        memset(variable, 0, sizeof(variable));

        strncpy(expression_copy, expressions[a], sizeof(expression_copy) - 1);
        expression_copy[sizeof(expression_copy) - 1] = '\0';

        name_pos = strstr(expression_copy, ":");
        if (name_pos != NULL) {
            *name_pos = '\0';
            trim_white_space(expression_copy);
            name_pos++;
        } else {
            name_pos = expression_copy;
        }

        delim = NULL;
        if (strstr(name_pos, "<=") != NULL) {
            delim = "<=";
        } else if (strstr(name_pos, ">=") != NULL) {
            delim = ">=";
        } else if (strstr(name_pos, "<") != NULL) {
            delim = "<";
        } else if (strstr(name_pos, ">") != NULL) {
            delim = ">";
        } else if (strstr(name_pos, "=") != NULL){
            delim = "=";
        } else {
            return 93;
        }

        delim_pos = strstr(name_pos, delim);
        if (delim_pos == NULL) {
            printf("Error: Delimiter not found.\n");
            return 93;
        }

        *delim_pos = '\0';

        left_side = remove_spaces(name_pos);
        right_side = trim_white_space(delim_pos + strlen(delim));

        normalize_expression(left_side);

        printf("Normalized express: %s\n", left_side);

        if((res_code = simplify_expression(left_side, simplified_expression))) {
            return res_code;
        }

        printf("Expanded express: %s\n", simplified_expression);

        j = 0;
        for (i = 0; simplified_expression[i] != '\0'; i++) {
            if (simplified_expression[i] == '-') {
                if (i > 0 && simplified_expression[i - 1] != '+' && simplified_expression[i - 1] != '-') {
                    modified_expression[j++] = '+';
                }
            }
            modified_expression[j++] = simplified_expression[i];
        }
        modified_expression[j] = '\0';

        printf("Modified: %s\n", modified_expression);

        token = strtok(modified_expression, "+");
        while (token != NULL) {
            remove_spaces(token);
            if (strlen(token) > 0 && extract_variable_and_coefficient(token, variable, &coefficient) == 0) {
                var_index = get_var_index(general_vars, variable);
                if (var_index == -1) {
                    printf("Unknown variable '%s'!\n", variable);
                    return 10;
                }

                general_vars->used_vars[var_index] = 1;

                /* Populate the simplex tableau */
                tableau->tableau[a][var_index] = coefficient;
            } else {
                return 93;
            }

            token = strtok(NULL, "+");
        }

        tableau->tableau[a][tableau->col_count - 1] = strtod(right_side, NULL);

        if (strstr(delim, "<")) {
            tableau->tableau[a][general_vars->num_general_vars + a] = 1;
        }
        else if (strstr(delim, ">")) {
            tableau->tableau[a][general_vars->num_general_vars + a] = -1;
            tableau->tableau[a][general_vars->num_general_vars + num_of_constraints + a] = 1;
            for (i = 0; i < tableau->col_count; i++) {
                if (i == general_vars->num_general_vars + num_of_constraints + a) {
                    /*tableau->tableau[tableau->row_count - 1][i] = 1;*/
                }
                else {
                    tableau->tableau[tableau->row_count - 1][i] += tableau->tableau[a][i];
                }
            }
        }
        else if (strstr(delim, "=")) {
            tableau->tableau[a][general_vars->num_general_vars + num_of_constraints + a] = 1;
            for (i = 0; i < tableau->col_count; i++) {
                if (i == general_vars->num_general_vars + num_of_constraints + a) {
                    /*tableau->tableau[tableau->row_count - 1][i] = 1;*/
                }
                else {
                    tableau->tableau[tableau->row_count - 1][i] += tableau->tableau[a][i];
                }
            }
        }
    }

    return 0;
}