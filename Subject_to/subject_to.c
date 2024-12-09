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

int split_expression(char *expression, char *name_pos, char **delim, char **left_side_expression, char **right_side_expression) {
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

    *left_side_expression = remove_spaces(name_pos);
    *right_side_expression = trim_white_space(delim_pos + strlen(*delim));

    return 0;
}

int parse_subject_to(char **expressions, int num_of_constraints, SimplexTableau *tableau, General_vars *general_vars) {
    char *left_side_expression = NULL;
    char *right_side_expression = NULL;
    char *delim = NULL;
    char *name_pos = NULL;
    char modified_expression[256];
    char simplified_expression[256];
    int i, result_code = 0;

    if(!expressions || !*expressions || !tableau || !general_vars) {
        return 93;
    }

    for (i = 0; i < num_of_constraints; i++) {
        memset(modified_expression, 0, sizeof(modified_expression));

        if((result_code = split_expression(expressions[i], name_pos, &delim, &left_side_expression, &right_side_expression))) {
            return result_code;
        }

        if(validate_expression(left_side_expression) || check_invalid_chars(left_side_expression, "^,:") || validate_expression(right_side_expression)) {
            return 11;
        }

        normalize_expression(left_side_expression);

        /*printf("Normalized express: %s\n", left_side);*/

        if((result_code = simplify_expression(left_side_expression, simplified_expression))) {
            return result_code;
        }

        /*printf("Expanded express: %s\n", simplified_expression);*/

        modify_expression(simplified_expression, modified_expression);

        /*printf("Modified: %s\n", modified_expression);*/

        if((result_code = insert_constraints_into_row(modified_expression, general_vars, tableau->tableau[i]))) {
            return result_code;
        }

        tableau->tableau[i][tableau->col_count - 1] = strtod(right_side_expression, NULL);
        introduce_additional_vars(tableau, delim, i, general_vars->num_general_vars, num_of_constraints);
    }

    return 0;
}