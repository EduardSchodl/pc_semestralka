#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "objectives.h"
#include "../Parse/parse.h"
#include "../Validate/validate.h"

int parse_objectives(char **expressions, SimplexTableau *tableau, General_vars *general_vars, double objective_row[], int num_lines) {
    char modified_expression[256];
    char simplified_expression[256];
    char expression[256];
    int i, result_code;

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

        if(validate_expression(expression) || check_invalid_chars(expression, "^,:")) {
            return 11;
        }

        normalize_expression(expression);

        /*printf("Normalized express: %s\n", expression);*/

        if((result_code = simplify_expression(expression, simplified_expression))) {
            return result_code;
        }

        /*printf("Expanded express: %s\n", simplified_expression);*/

        modify_expression(simplified_expression, modified_expression);

        /*printf("Modified: %s\n", modified_expression);*/

        if((result_code = insert_constraints_into_row(modified_expression, general_vars, objective_row))) {
            return result_code;
        }
    }

    return 0;
}
