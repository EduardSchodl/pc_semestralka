#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "validate.h"
#include "../Generals/generals.h"
#include "../Consts/error_codes.h"
#include "../Consts/constants.h"

int is_var_known(const General_vars *general_vars, const char *var_name) {
    int i;

    /* sanity check */
    if (!general_vars || !var_name) {
        return SANITY_CHECK_ERROR;
    }

    /* kontrola, zda promměná existuje v sekci Generals */
    for (i = 0; i < general_vars->num_general_vars; i++) {
        if (strcmp(var_name, general_vars->general_vars[i]) == 0) {
            return 0;
        }
    }

    /* proměnná neexistuje */
    printf("Unknown variable '%s'!\n", var_name);
    return UNKNOWN_VARIABLE;
}

int is_valid_string(const char *str) {
    int i;
    char invalid_chars[] = INVALID_STRING_CHARS;

    /* sanity check */
    if (!str) {
        return 1;
    }

    /* kontrola, zda řetězec obsahuje nepovolené znaky */
    if (strpbrk(str, invalid_chars)) {
        return 1;
    }

    /* kontrola, zda řetězec začíná neplatným znakem */
    if (!is_var_start(str[0])) {
        return 1;
    }

    /* kontrola, zda řetězec obsahuje neplatné znaky */
    for (i = 0; invalid_chars[i] != '\0'; i++) {
        if (!is_var_part(str[0])) {
            return 1;
        }
    }

    /* řetězec je v pořádku */
    return 0;
}

int contains_invalid_operator_sequence(char *str) {
    /* sanity check */
    if (!str) {
        return 1;
    }

    /* kontrola neplatné sekvence operátorů */
    if ((strstr(str, "<") || strstr(str, ">") || strstr(str, "=")) && strstr(str, "free")) {
        return 1;
    }

    return 0;
}

int bounds_valid_operators(const char *str) {
    const char invalid_chars[] = BOUNDS_INVALID_CHARS;
    char *res;

    /* sanity check */
    if (!str) {
        return 1;
    }

    /* kontrola, zda řetězec v sekci Bounds obsahuje neplatné znaky */
    res = strpbrk(str, invalid_chars);
    if (res) {
        return 1;
    }

    /* kontrola, zda v řetězci chybí operátory */
    if (!(strstr(str, "<=") || strstr(str, ">=") || strstr(str, "<") || strstr(str, ">") || strstr(str, "free"))) {
        return 1;
    }

    return 0;
}

int check_unused_variables(General_vars *general_vars) {
    int i;

    /* sanity check */
    if (!general_vars) {
        return 1;
    }

    /* kontrola, zda byly všechny proměnné použity */
    for (i = 0; i < general_vars->num_general_vars; i++) {
        if (!general_vars->used_vars[i]) {
            printf("Warning: unused variable '%s'!\n", general_vars->general_vars[i]);
        }
    }

    return 0;
}

int check_invalid_chars(char *line, char *invalid_chars) {
    /* kontrola, zda řetězec obsahuje některý ze zadaných neplatných operátorů */
    if (strpbrk(line, invalid_chars)) {
        return 1;
    }

    return 0;
}

int is_operator(char c) {
    /* kontrola, zda je znak operátor */
    return c == '+' || c == '-' || c == '*' || c == '=' || c == '<' || c == '>';
}

int is_var_start(char c) {
    /* kontrola, zda znak může být začátkem názvu proměnné */
    return isalpha(c) || c == '@' || c == '_' || c == '$';
}

int is_var_part(char c) {
    /* kontrola, zda znak může být součástí názvu proměnné */
    return (isalnum(c) || c == '_' || c == '@' || c == '$') && !strchr("+-*^<>=()[]{}.,:", c);
}

int validate_expression(const char *expression) {
    int i;
    char prev = '\0';
    char stack[MAX_STACK_SIZE];
    int stack_top = 0;
    char top;
    char c;

    /* sanity check */
    if (!expression) {
        return 1;
    }

    /* procházení výrazu znak po znaku */
    for (i = 0; expression[i] != '\0'; i++) {
        c = expression[i];

        /* kontrola párových závorek, zda sedí typy závorek */
        if (c == '(' || c == '[' || c == '{') {
            stack[stack_top++] = c;
        } else if (c == ')' || c == ']' || c == '}') {
            if (stack_top <= 0) {
                return 1;
            }

            top = stack[--stack_top];

            if ((c == ')' && top != '(') ||
                (c == ']' && top != '[') ||
                (c == '}' && top != '{')) {
                return 1;
            }

            if (is_operator(prev)) {
                return 1;
            }
        }

        /* kontrola záporných čísel za operátory */
        if (c == '-') {
            if (i == 0 || prev == '<' || prev == '>' || prev == '=') {
                continue;
            }

            if (is_operator(prev) && !(prev == '>' || prev == '<' || prev == '=')) {
                return 1;
            }
        }

        /* kontrola sekvence operátorů */
        if (is_operator(c) && ((is_operator(prev) && (prev != '<' && prev != '>')))) {
            return 1;
        }

        /* kontrola, zda operátor neuzavírá závorku nebo výraz */
        if (is_operator(prev) && (c == ')' || c == '\0')) {
            return 1;
        }

        /* kontrola, zda je znak neplatný */
        if (!is_var_part(c) && !is_operator(c) && c != '(' && c != ')' && c != '_' && c != '[' && c != ']' && c != '{'
            && c != '}') {
            return 1;
        }

        prev = c;
    }

    /* kontrola, zda byly všechny závorky uzavřeny */
    if (stack_top != 0) {
        return 1;
    }

    /* kontrola, zda výraz nekončí operátorem */
    if (is_operator(prev)) {
        return 1;
    }

    return 0;
}
