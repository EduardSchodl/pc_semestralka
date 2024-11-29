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
    char invalid_chars[] = "+-*^<>=()[],:";

    if(!str) {
        return 1;
    }

    if(strpbrk(str, invalid_chars)) {
        return 1;
    }

    if (!isalpha((unsigned char)str[0])) {
        return 1;
    }

    for (i = 1; str[i] != '\0'; i++) {
        if (!isalnum((unsigned char)str[i]) && str[i] != '_') {
            return 1;
        }
    }

    return 0;
}

int is_valid_operator_char(char c) {
    return c == '<' || c == '>' || c == '=';
}

int is_valid_operator(const char *str) {
    if(!str) {
        return 1;
    }

    if (strlen(str) == 1) {
        return is_valid_operator_char(str[0]);
    } else if (strlen(str) == 2) {
        return (str[0] == '<' && str[1] == '=') || (str[0] == '>' && str[1] == '=');
    }
    return 0;
}

int contains_only_valid_operators(const char *str) {
    const char *p = str;

    if(!str) {
        return 1;
    }

    while (*p) {
        if (is_valid_operator(p)) {
            p += (strlen(p) > 1 && (*(p + 1) == '=')) ? 2 : 1;
        } else{
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
    const char invalid_chars[] = "(){}[]/*+-";
    char *res;

    if(!str) {
        return 1;
    }

    res = strpbrk(str, invalid_chars);
    if(res){
        return 1;
    }

    if (strstr(str, "<=") || strstr(str, ">=") || strstr(str, "<") || strstr(str, ">")) {
        return 0;
    }

    return 1;
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