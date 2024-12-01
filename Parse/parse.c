#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Validate/validate.h"
#include "parse.h"

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
    int i, idx;
    char c;
    int reading_coefficient = 0;
    int reading_decimal = 0;
    double decimal_divisor = 1.0;
    double multiplier_stack[100];
    double current_multiplier = 1.0;
    char buffer[256];
    int buffer_len = 0;

    if (!expression || !simplified_expression) {
        return 93;
    }

    if (check_matching_parentheses(expression)) {
        printf("Syntax error: unmatched parentheses!\n");
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
                decimal_divisor *= 10.0;
                coefficient += (c - '0') / decimal_divisor;
            } else {
                coefficient = coefficient * 10.0 + (c - '0');
            }
        } else if (c == '.') {
            reading_decimal = 1;
        } else if (c == '(') {
            if (!reading_coefficient) coefficient = 1.0;

            current_multiplier *= coefficient * sign * neg_stack[stack_top - 1];
            multiplier_stack[stack_top] = current_multiplier;
            neg_stack[stack_top] = neg_stack[stack_top - 1];
            stack_top++;

            coefficient = 0;
            reading_coefficient = 0;
            sign = 1;
        } else if (c == ')') {
            stack_top--;
            current_multiplier = multiplier_stack[stack_top - 1];
        } else if (isalpha(c) || c == '_') {
            idx = 0;
            while (isalnum(c) || c == '_') {
                variable[idx++] = c;
                c = expression[++i];
            }
            variable[idx] = '\0';
            i--;

            if (!reading_coefficient) coefficient = 1.0;

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