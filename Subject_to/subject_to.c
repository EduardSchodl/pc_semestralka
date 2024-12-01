#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "subject_to.h"
#include "../Parse/parse.h"

char* identify_delimiter(const char* expression) {
    if (strstr(expression, "<=")) return "<=";
    if (strstr(expression, ">=")) return ">=";
    if (strstr(expression, "<")) return "<";
    if (strstr(expression, ">")) return ">";
    if (strstr(expression, "=")) return "=";
    return NULL;
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
    char simplified_expression[256];
    int j, i, var_index, a;
    int res_code = 0;

    if(!expressions || !*expressions || !tableau || !general_vars) {
        return 93;
    }

    for (a = 0; a < num_of_constraints; a++) {
        memset(modified_expression, 0, sizeof(modified_expression));
        memset(variable, 0, sizeof(variable));

        name_pos = strstr(expressions[a], ":");
        if (name_pos != NULL) {
            *name_pos = '\0';
            trim_white_space(expressions[a]);
            name_pos++;
        } else {
            name_pos = expressions[a];
        }

        delim = identify_delimiter(name_pos);
        if (!delim) {
            printf("Error: Delimiter not found.\n");
            return 93;
        }

        delim_pos = strstr(name_pos, delim);
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