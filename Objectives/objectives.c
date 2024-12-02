#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "objectives.h"
#include "../Parse/parse.h"

int parse_objectives(char **expressions, SimplexTableau *tableau, General_vars *general_vars, double objective_row[], int num_lines) {
    char modified_expression[256];
    char simplified_expression[256];
    char expression[256];
    int i, res_code;

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

        modify_expression(simplified_expression, modified_expression);

        if((res_code = insert_constraints_into_row(modified_expression, general_vars, objective_row))) {
            return res_code;
        }
    }

    return 0;
}
