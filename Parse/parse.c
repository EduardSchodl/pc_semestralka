#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"
#include "../Validate/validate.h"
#include "../LProblem/lp.h"
#include "../Consts/error_codes.h"
#include "../Consts/constants.h"


char *trim_white_space(char *str) {
    char *end;

    /* sanity check */
    if (!str) {
        return NULL;
    }

    /* odstranění počátečních bílých znaků */
    while (*str && isspace((unsigned char) *str)) {
        str++;
    }

    if (*str == '\0') return str;

    /* nalezení konce řetězce a odstranění koncových bílých znaků */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) {
        end--;
    }

    *(end + 1) = '\0';

    return str;
}

char *remove_spaces(char *str) {
    int count = 0, i;

    /* sanity check */
    if (!str) {
        return NULL;
    }

    /* odstranění mezer */
    for (i = 0; str[i]; i++) {
        if (str[i] != ' ') {
            str[count++] = str[i];
        }
    }

    /* nakonec ukončovací znak */
    str[count] = '\0';

    return str;
}

void modify_expression(char *expression, char *modified_expression) {
    int j = 0, i;

    /* sanity check */
    if (!expression) {
        return;
    }

    /* vložení '+' před záporné hodnoty pro lepší parsování */
    for (i = 0; expression[i] != '\0'; i++) {
        if (expression[i] == '-') {
            if (i > 0 && expression[i - 1] != '+' && expression[i - 1] != '-') {
                modified_expression[j++] = '+';
            }
        }
        modified_expression[j++] = expression[i];
    }

    /* ukončení řetězce */
    modified_expression[j] = '\0';
}

double parse_coefficient(const char *token) {
    char *end_ptr;
    double coeff = strtod(token, &end_ptr);

    /* sanity check */
    if (!token) {
        return 0;
    }

    /* pokud není číslo, je číslem 1 nebo -1 */
    if (end_ptr == token) {
        return (token[0] == '-') ? -1.0 : 1.0;
    }

    return coeff;
}

int extract_variable_and_coefficient(char *segment, char *variable, double *coefficient) {
    char *segment_ptr = segment;
    char coeff_buffer[MAX_COEFF_BUFFER] = {0};
    int i = 0, index;
    char *exponent_ptr;

    /* sanity check */
    if (!segment || !variable) {
        return 1;
    }

    /* nalezení a odstranění znaku '*', pokud existuje */
    exponent_ptr = strchr(segment_ptr, '*');

    if (exponent_ptr) {
        index = (int) (exponent_ptr - segment_ptr);
        memmove(&segment_ptr[index], &segment_ptr[index + 1], strlen(segment_ptr) - index);
    }

    /* načítání koeficientu */
    while (*segment_ptr && (isdigit(*segment_ptr) || *segment_ptr == '.' || *segment_ptr == '-')) {
        coeff_buffer[i++] = *segment_ptr;
        segment_ptr++;
    }

    /* parsování koeficientu pokud je, jinak výchozí 1 */
    if (i > 0) {
        *coefficient = parse_coefficient(coeff_buffer);
    } else {
        *coefficient = 1.0;
    }

    /* přeskočení '*' */
    if (*segment_ptr == '*') {
        segment_ptr++;
    }

    /* uložení koeficientu */
    strcpy(variable, segment_ptr);

    /*printf("Segment: %s | Variable: %s | Coefficient: %f\n", segment, variable, *coefficient);*/

    /* kontrola, zda je název proměnné validní */
    return is_valid_string(variable) ? 1 : 0;
}

void normalize_expression(char *expression) {
    int i;

    /* sanity check */
    if (!expression) {
        return;
    }

    /* nahrazení závorek za kulaté */
    for (i = 0; expression[i]; i++) {
        if (expression[i] == '{' || expression[i] == '[') {
            expression[i] = '(';
        }
        if (expression[i] == '}' || expression[i] == ']') {
            expression[i] = ')';
        }
    }
}

int check_matching_parentheses(const char *expression) {
    int counter = 0;
    int i;

    /* sanity check */
    if (!expression) {
        return 1;
    }

    /* kontrola párovosti závorek */
    for (i = 0; expression[i] != '\0'; i++) {
        if (expression[i] == '(') {
            counter++;
        } else if (expression[i] == ')') {
            counter--;
            if (counter < 0) {
                return 1;
            }
        }
    }

    /* counter musí být 0, jinak chyba */
    return counter == 0 ? 0 : 1;
}

void add_term(Term terms[], int *term_count, double coefficient, const char *variable) {
    int i;

    /* sanity check */
    if (!terms || !variable) {
        return;
    }

    /* pokud již proměnná existuje, přičte koeficient */
    for (i = 0; i < *term_count; i++) {
        if (strcmp(terms[i].variable, variable) == 0) {
            terms[i].coefficient += coefficient;
            return;
        }
    }

    /* pokud proměnná neexistuje, vytvoří ji v poli a nastaví koeficient */
    terms[*term_count].coefficient = coefficient;
    strcpy(terms[*term_count].variable, variable);
    (*term_count)++;
}

void process_term(Term terms[], int *term_count, double coefficient, int sign, char *variable) {
    /* sanity check */
    if (!terms || !variable) {
        return;
    }

    /* zpracování proměnné, pokud existuje */
    if (variable[0] != '\0') {
        add_term(terms, term_count, coefficient * sign, variable);
        variable[0] = '\0';
    }
}

int simplify_expression(const char *expression, char *simplified_expression) {
    Term terms[MAX_TERMS];
    int term_count = 0;
    double coefficient = 0;
    int sign = 1;
    int neg_stack[MAX_STACK_SIZE];
    int stack_top = 0;
    char variable[MAX_VAR_NAME];
    int i, index;
    char c, prev = '\0';
    int reading_coefficient = 0;
    int reading_decimal = 0;
    double decimal_divisor = 1.0;
    double multiplier_stack[MAX_STACK_SIZE];
    double current_multiplier = 1.0;
    char buffer[MAX_BUFFER_SIZE];
    int buffer_len = 0;

    /* sanity check */
    if (!expression || !simplified_expression) {
        return SANITY_CHECK_ERROR;
    }

    /* kontrola párovosti závorek */
    if (check_matching_parentheses(expression)) {
        return SYNTAX_ERROR;
    }

    /* inicializace zásobníku */
    stack_top = 0;
    neg_stack[stack_top++] = 1;
    multiplier_stack[stack_top - 1] = 1.0;

    /* prochází výraz znak po znaku */
    for (i = 0; expression[i] != '\0'; i++) {
        c = expression[i];

        /* kontrola neplatných operátorů */
        if ((prev == '*' && c == '*') ||
            (prev == '*' && !is_var_start(c) && !isdigit(c) && c != '(') ||
            (c == '*' && (i == 0 || expression[i + 1] == '\0'))) {
            return SYNTAX_ERROR;
        }

        /* čtení čísla */
        if (isdigit(c)) {
            if (!reading_coefficient) {
                coefficient = 0;
                reading_coefficient = 1;
                reading_decimal = 0;
                decimal_divisor = 1.0;
            }
            /* čtení desetinné části */
            if (reading_decimal) {
                decimal_divisor *= 10.0;
                coefficient += (c - '0') / decimal_divisor;
            } else {
                coefficient = coefficient * 10.0 + (c - '0');
            }
        } else if (c == '.') {
            /* začátek čtení desetinné části */
            reading_decimal = 1;
        } else if (c == '(') {
            if (!reading_coefficient) coefficient = 1.0;

            /* správa zásobníku pro multiplikaci */
            current_multiplier *= coefficient * sign * neg_stack[stack_top - 1];
            multiplier_stack[stack_top] = current_multiplier;
            neg_stack[stack_top] = neg_stack[stack_top - 1];
            stack_top++;

            coefficient = 0;
            reading_coefficient = 0;
            sign = 1;
        } else if (c == ')') {
            /* uzavření multiplikace */
            stack_top--;
            current_multiplier = multiplier_stack[stack_top - 1];
        } else if (is_var_start(c)) {
            index = 0;

            /* čtení proměnné */
            while (is_var_part(c)) {
                variable[index++] = c;
                c = expression[++i];
            }

            variable[index] = '\0';
            i--;

            if (!reading_coefficient) coefficient = 1.0;

            /* zpracování termínu */
            process_term(terms, &term_count, coefficient * current_multiplier, sign * neg_stack[stack_top - 1],
                         variable);

            coefficient = 0;
            reading_coefficient = 0;
        } else if (c == '*') {
            /* přeskočení násobení */
            continue;
        } else if (c == '+') {
            /* kontrola neplatné posloupnosti znaků */
            if (prev == '*' || prev == '+' || prev == '-') {
                return SYNTAX_ERROR;
            }

            /* zpracování termínu */
            if (reading_coefficient) {
                process_term(terms, &term_count, coefficient * current_multiplier, sign * neg_stack[stack_top - 1], "");
            }

            coefficient = 0;
            sign = 1;
            reading_coefficient = 0;
        } else if (c == '-') {
            /* kontrola neplatné posloupnosti znaků */
            if (prev == '*' || prev == '+' || prev == '-') {
                return SYNTAX_ERROR;
            }

            /* zpracování termínu */
            if (reading_coefficient) {
                process_term(terms, &term_count, coefficient * current_multiplier, sign * neg_stack[stack_top - 1], "");
            }

            coefficient = 0;
            sign = -1;
            reading_coefficient = 0;
        }

        prev = expression[i];
    }

    /* zpracování posledního termínu */
    if (reading_coefficient) {
        process_term(terms, &term_count, coefficient * current_multiplier, sign * neg_stack[stack_top - 1], "");
    }

    buffer_len = 0;
    /* složení zjednodušeného výrazu */
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

    /* uložení zpracovaného výrazu */
    strcpy(simplified_expression, buffer);

    return 0;
}
