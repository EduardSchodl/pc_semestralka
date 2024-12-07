#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "subject_to.h"
#include "../Parse/parse.h"
#include "../Validate/validate.h"

char* identify_delimiter(const char* expression) {
    if (strstr(expression, "<=")) return "<=";
    if (strstr(expression, ">=")) return ">=";
    if (strstr(expression, "<")) return "<";
    if (strstr(expression, ">")) return ">";
    if (strstr(expression, "=")) return "=";
    return NULL;
}

void introduce_additional_vars(SimplexTableau *tableau, char *delim, int row, int col, int num_of_constraints) {
    int i;

    if(!tableau || !delim) {
        return;
    }

    if (strstr(delim, "<")) {
        tableau->tableau[row][col + row] = 1;
    }
    else if (strstr(delim, ">")) {
        tableau->tableau[row][col + row] = -1;
        tableau->tableau[row][col + num_of_constraints + row] = 1;
        for (i = 0; i < tableau->col_count; i++) {
            if (i == col + num_of_constraints + row) {
                /*tableau->tableau[tableau->row_count - 1][i] = 1;*/
            }
            else {
                tableau->tableau[tableau->row_count - 1][i] += tableau->tableau[row][i];
            }
        }
    }
    else if (strstr(delim, "=")) {
        tableau->tableau[row][col + num_of_constraints + row] = 1;
        for (i = 0; i < tableau->col_count; i++) {
            if (i == col + num_of_constraints + row) {
                /*tableau->tableau[tableau->row_count - 1][i] = 1;*/
            }
            else {
                tableau->tableau[tableau->row_count - 1][i] += tableau->tableau[row][i];
            }
        }
    }
}

int split_expression(char *expression, char *name_pos, char **delim, char **left, char **right) {
    char *delim_pos;

    if(!expression) {
        return 93;
    }

    name_pos = strstr(expression, ":");
    if (name_pos != NULL) {
        *name_pos = '\0';
        trim_white_space(expression);
        name_pos++;
    } else {
        name_pos = expression;
    }

    *delim = identify_delimiter(name_pos);
    if (!*delim) {
        printf("Error: Delimiter not found.\n");
        return 93;
    }

    delim_pos = strstr(name_pos, *delim);
    *delim_pos = '\0';

    *left = remove_spaces(name_pos);
    *right = trim_white_space(delim_pos + strlen(*delim));

    return 0;
}

int parse_subject_to(char **expressions, int num_of_constraints, SimplexTableau *tableau, General_vars *general_vars) {
    char *left_side = NULL;
    char *right_side = NULL;
    char *delim = NULL;
    char *name_pos = NULL;
    char modified_expression[256];
    char simplified_expression[256];
    int a, res_code = 0;

    if(!expressions || !*expressions || !tableau || !general_vars) {
        return 93;
    }

    for (a = 0; a < num_of_constraints; a++) {
        memset(modified_expression, 0, sizeof(modified_expression));

        if((res_code = split_expression(expressions[a], name_pos, &delim, &left_side, &right_side))) {
            return res_code;
        }

        normalize_expression(left_side);

        /*printf("Normalized express: %s\n", left_side);*/

        if(validate_expression(left_side) || check_invalid_chars(left_side, "^,:") || validate_expression(right_side)) {
            return 11;
        }

        if((res_code = simplify_expression(left_side, simplified_expression))) {
            return res_code;
        }

        /*printf("Expanded express: %s\n", simplified_expression);*/

        modify_expression(simplified_expression, modified_expression);

        /*printf("Modified: %s\n", modified_expression);*/

        if((res_code = insert_constraints_into_row(modified_expression, general_vars, tableau->tableau[a]))) {
            return res_code;
        }

        tableau->tableau[a][tableau->col_count - 1] = strtod(right_side, NULL);
        introduce_additional_vars(tableau, delim, a, general_vars->num_general_vars, num_of_constraints);
    }

    return 0;
}