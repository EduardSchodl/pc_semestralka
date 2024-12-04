#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "validate.h"
#include "../Generals/generals.h"

int is_var_known(const General_vars *general_vars, const char *var_name){
    int i;

    if(!general_vars || !var_name) {
        return 93;
    }

    for(i = 0; i < general_vars->num_general_vars; i++) {
        if(strcmp(var_name, general_vars->general_vars[i]) == 0) {
            return 0;
        }
    }

    printf("Unknown variable '%s'!\n", var_name);
    return 10;
}

int is_valid_string(const char *str) {
    int i;
    char invalid_chars[] = "+-*^<>=()[].,:";

    if(!str) {
        return 1;
    }

    if(strpbrk(str, invalid_chars)) {
        return 1;
    }

    if (!is_var_start(str[0])) {
        return 1;
    }

    for (i = 0; invalid_chars[i] != '\0'; i++) {
        if (!is_var_part(str[0])) {
            return 1;
        }
    }

    return 0;
}

int contains_invalid_operator_sequence(char *str) {
    if(!str) {
        return 1;
    }

    if((strstr(str, "<") || strstr(str, ">")) && strstr(str, "free")) {
        return 1;
    }

    return 0;
}

int bounds_valid_operators(const char *str) {
    const char invalid_chars[] = "(){}[]/*+";
    char *res;

    if(!str) {
        return 1;
    }

    res = strpbrk(str, invalid_chars);
    if(res){
        return 1;
    }

    if (!(strstr(str, "<=") || strstr(str, ">=") || strstr(str, "<") || strstr(str, ">") || strstr(str, "free"))) {
        return 1;
    }

    return 0;
}

int check_unused_variables(General_vars *general_vars) {
    int i;

    if(!general_vars) {
        return 1;
    }

    for(i = 0; i < general_vars->num_general_vars; i++) {
        if(!general_vars->used_vars[i]) {
            printf("Warning: unused variable '%s'!\n", general_vars->general_vars[i]);
            return 1;
        }
    }

    return 0;
}

int check_invalid_chars(char *line, char *invalid_chars) {
    if(strpbrk(line, invalid_chars)) {
        printf("Syntax error!\n");
        return 1;
    }

    return 0;
}

int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '=' || c == '<' || c == '>';
}

int is_var_start(char c) {
    return isalpha(c) || c == '@' || c == '_' || c == '$';
}

int is_var_part(char c) {
    return (isalnum(c) || c == '_' || c == '@' || c == '$') && !strchr("+-*^<>=()[].,:", c);
}

int validate_expression(const char *expression) {
    int i;
    char prev = '\0';
    int paren_stack = 0;
    int res_code = 0;

    if (!expression) {
        res_code = 11;
        return res_code;
    }

    for (i = 0; expression[i] != '\0'; i++) {
        char c = expression[i];

        if (c == '(') {
            paren_stack++;
        } else if (c == ')') {
            if (paren_stack <= 0) {
                res_code = 11;
            }
            paren_stack--;
            if (is_operator(prev)) {
                res_code = 11;
            }
        }

        if (is_operator(c) && (prev == '\0' || is_operator(prev) || prev == '(')) {
            res_code = 11;
        }

        if (is_operator(prev) && (c == ')' || c == '\0')) {
            res_code = 11;
        }

        if (!isalnum(c) && !is_operator(c) && c != '(' && c != ')' && c != '_') {
            res_code = 11;
        }

        prev = c;
    }

    if (paren_stack != 0) {
        res_code = 11;
    }

    if (is_operator(prev)) {
        res_code = 11;
    }

    if(res_code) {
        printf("Syntax error!\n");
    }

    return res_code;
}