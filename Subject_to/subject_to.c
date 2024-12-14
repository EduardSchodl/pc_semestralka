#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "subject_to.h"
#include "../Parse/parse.h"
#include "../Validate/validate.h"
#include "../Consts/error_codes.h"
#include "../Consts/constants.h"

char *identify_delimiter(const char *expression) {
    /* sanity check */
    if (!expression) {
        return NULL;
    }

    /* identifikace delimiteru */
    if (strstr(expression, "<=")) return "<=";
    if (strstr(expression, ">=")) return ">=";
    if (strstr(expression, "<")) return "<";
    if (strstr(expression, ">")) return ">";
    if (strstr(expression, "=")) return "=";

    return NULL;
}

void introduce_additional_vars(SimplexTableau *tableau, char *delim, int row, int col, int num_of_constraints) {
    int i;

    /* sanity check */
    if (!tableau || !delim) {
        return;
    }

    /* zpracování hodnot tabulky pro jednotlivé typy delimiterů */
    if (strstr(delim, "<")) {
        /* přidání slack proměnné */
        tableau->tableau[row][col + row] = 1;
    } else if (strstr(delim, ">")) {
        /* přidání surplus proměnné */
        tableau->tableau[row][col + row] = -1;

        /* přidání artificial proměnné */
        tableau->tableau[row][col + num_of_constraints + row] = 1;

        /* vložení hodnoty do objektové řádky */
        for (i = 0; i < tableau->col_count; i++) {
            if (i == col + num_of_constraints + row) {
                /*tableau->tableau[tableau->row_count - 1][i] = 1;*/
            } else {
                tableau->tableau[tableau->row_count - 1][i] += tableau->tableau[row][i];
            }
        }
    } else if (strstr(delim, "=")) {
        /* přidání artificial proměnné */
        tableau->tableau[row][col + num_of_constraints + row] = 1;

        /* vložení hodnoty do objektové řádky */
        for (i = 0; i < tableau->col_count; i++) {
            if (i == col + num_of_constraints + row) {
                /*tableau->tableau[tableau->row_count - 1][i] = 1;*/
            } else {
                tableau->tableau[tableau->row_count - 1][i] += tableau->tableau[row][i];
            }
        }
    }
}

int split_expression(char *expression, char *name_pos, char **delim, char **left_side_expression,
                     char **right_side_expression) {
    char *delim_pos;

    /* sanity check */
    if (!expression) {
        return SANITY_CHECK_ERROR;
    }

    /* kontrola, zda řádka obsahuje volitelný název */
    name_pos = strstr(expression, ":");
    if (name_pos != NULL) {
        *name_pos = '\0';
        trim_white_space(expression);
        name_pos++;
    } else {
        name_pos = expression;
    }

    /* identifikace delimiteru */
    *delim = identify_delimiter(name_pos);
    if (!*delim) {
        printf("Error: Delimiter not found.\n");
        return PARSING_ERROR;
    }

    /* nalezení pozice delimiteru */
    delim_pos = strstr(name_pos, *delim);
    *delim_pos = '\0';

    /* rozdělení a zpracování levé a pravé části výrazu */
    *left_side_expression = remove_spaces(name_pos);
    *right_side_expression = trim_white_space(delim_pos + strlen(*delim));

    return 0;
}

int parse_subject_to(char **expressions, int num_of_constraints, SimplexTableau *tableau, General_vars *general_vars) {
    char *left_side_expression = NULL;
    char *right_side_expression = NULL;
    char *delim = NULL;
    char *name_pos = NULL;
    char modified_expression[MAX_BUFFER_SIZE];
    char simplified_expression[MAX_BUFFER_SIZE];
    int i, result_code = 0;

    /* sanity check */
    if (!expressions || !*expressions || !tableau || !general_vars) {
        return SANITY_CHECK_ERROR;
    }

    /* zpracování jednotlivých řádek sekce Subject To */
    for (i = 0; i < num_of_constraints; i++) {
        memset(modified_expression, 0, sizeof(modified_expression));

        /* rozdělení výrazu na část před a po delimiteru */
        if ((result_code = split_expression(expressions[i], name_pos, &delim, &left_side_expression,
                                            &right_side_expression))) {
            return result_code;
        }

        /* kontrola validity výrazu */
        if (validate_expression(left_side_expression) || check_invalid_chars(left_side_expression, "^,:")
            || validate_expression(right_side_expression) || check_invalid_chars(right_side_expression, "=<>^/")) {
            return SYNTAX_ERROR;
        }

        /* normalizace výrazu, sjednocení typů závorek */
        normalize_expression(left_side_expression);

        /*printf("Normalized express: %s\n", left_side);*/

        /* zjednodušení výrazu */
        if ((result_code = simplify_expression(left_side_expression, simplified_expression))) {
            return result_code;
        }

        /*printf("Expanded express: %s\n", simplified_expression);*/

        /* modifikace výrazu, přidání znaků '+' před záporné hodnoty kvůli lepšímu parsování */
        modify_expression(simplified_expression, modified_expression);

        /*printf("Modified: %s\n", modified_expression);*/

        /* uložení výrazu na zadané místo v tabulce */
        if ((result_code = insert_constraints_into_row(modified_expression, general_vars, tableau->tableau[i]))) {
            return result_code;
        }

        /* vložení pravé strany výrazu do tabulky */
        tableau->tableau[i][tableau->col_count - 1] = strtod(right_side_expression, NULL);

        /* přidání dodatečných proměnných pro ošetření nerovností */
        introduce_additional_vars(tableau, delim, i, general_vars->num_general_vars, num_of_constraints);
    }

    return 0;
}
