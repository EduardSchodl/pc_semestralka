#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "objectives.h"
#include "../Parse/parse.h"

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
