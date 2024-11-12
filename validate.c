#include <string.h>
#include <ctype.h>
#include "validate.h"

#include <stdio.h>

int is_var_known(const General_vars *general_vars, const char *var_name){
    int i;

    for(i = 0; i < general_vars->num_general_vars; i++) {
        if(strcmp(var_name, general_vars->general_vars[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

int is_invalid_string(const char *str) {
    int i;

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
    if (strlen(str) == 1) {
        return is_valid_operator_char(str[0]);
    } else if (strlen(str) == 2) {
        return (str[0] == '<' && str[1] == '=') || (str[0] == '>' && str[1] == '=');
    }
    return 0;
}

int contains_only_valid_operators(const char *str) {
    const char *p = str;

    while (*p) {
        if (is_valid_operator(p)) {
            p += (strlen(p) > 1 && (*(p + 1) == '=')) ? 2 : 1;
        } else{
            return 0;
        }
    }
    return 1;
}

int contains_invalid_operator_sequence(char *str) {
    if((strstr(str, "<") || strstr(str, ">")) && strstr(str, "free")) {
        return 1;
    }

    return 0;
}

int bounds_valid_operators(const char *str) {
    const char invalid_chars[] = "(){}[]/*";
    char *res;

    res = strpbrk(str, invalid_chars);
    if(res){
        return 1;
    }

    if (strstr(str, "<=") || strstr(str, ">=") || strstr(str, "<") || strstr(str, ">")) {
        return 0;
    }

    return 1;
}