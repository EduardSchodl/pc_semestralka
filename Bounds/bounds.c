#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bounds.h"
#include "../Parse//parse.h"
#include "../Validate//validate.h"
#include "../Memory_manager/memory_manager.h"
#include "../Consts/error_codes.h"
#include "../Consts/constants.h"

Bounds *create_bounds(const int initial_size) {
    int i;
    Bounds *temp;

    /* sanity check */
    if (!initial_size) {
        return NULL;
    }

    temp = (Bounds *) tracked_malloc(sizeof(Bounds));
    if (!temp) {
        return NULL;
    }

    temp->lower_bound = tracked_malloc(initial_size * sizeof(double));
    if (!temp->lower_bound) {
        tracked_free(temp);
        return NULL;
    }

    temp->upper_bound = tracked_malloc(initial_size * sizeof(double));
    if (!temp->upper_bound) {
        tracked_free(temp->lower_bound);
        tracked_free(temp);
        return NULL;
    }

    for (i = 0; i < initial_size; i++) {
        temp->lower_bound[i] = DEFAULT_LOWER_BOUND;
        temp->upper_bound[i] = DEFAULT_UPPER_BOUND;
    }

    temp->num_vars = 0;
    temp->max_vars = initial_size;

    return temp;
}

void free_bounds(Bounds *bounds) {
    /* uvolnění struktury Bounds */
    if (bounds) {
        tracked_free(bounds->lower_bound);
        tracked_free(bounds->upper_bound);
        tracked_free(bounds);
    }
}

void add_bound(Bounds *bounds, const double lower_bound, const double upper_bound, int var_index) {
    int new_size, i;
    double *new_lower_bound, *new_upper_bound;

    /* sanity check */
    if (!bounds) {
        return;
    }

    /* pokud index proměnné přesáhne maximální velikost, zvětší se pole proměnných v Bounds */
    if (var_index >= bounds->max_vars) {
        new_size = var_index + 10;

        new_lower_bound = tracked_realloc(bounds->lower_bound, new_size * sizeof(double));
        if (!new_lower_bound) {
            return;
        }
        bounds->lower_bound = new_lower_bound;

        new_upper_bound = tracked_realloc(bounds->upper_bound, new_size * sizeof(double));
        if (!new_upper_bound) {
            return;
        }
        bounds->upper_bound = new_upper_bound;

        for (i = bounds->max_vars; i < new_size; i++) {
            bounds->lower_bound[i] = DEFAULT_LOWER_BOUND;
            bounds->upper_bound[i] = DEFAULT_UPPER_BOUND;
        }

        bounds->max_vars = new_size;
    }

    /* přidání proměnné do pole */
    bounds->lower_bound[var_index] = lower_bound;
    bounds->upper_bound[var_index] = upper_bound;

    if (var_index >= bounds->num_vars) {
        bounds->num_vars = var_index + 1;
    }
}

int parse_bounds(Bounds **bounds, General_vars *general_vars, char **lines, int num_lines) {
    char *tokens[MAX_TOKENS];
    int token_count = 0;
    char *ptr = NULL;
    char *operator_position = NULL;
    int i, j, result_code;
    double value;
    double lower_bound = DEFAULT_LOWER_BOUND;
    double upper_bound = DEFAULT_UPPER_BOUND;
    char var_name[MAX_VAR_NAME] = {0};
    char *line = NULL;
    int *processed_variables;
    int var_index, length;

    /* sanity check */
    if (!lines || !general_vars) {
        return SANITY_CHECK_ERROR;
    }

    /* vytvoření struktury Bounds */
    *bounds = create_bounds(general_vars->num_general_vars);
    if (!*bounds) {
        return MEMORY_ERROR;
    }

    /* alokace pro pole příznaků zpracovaných proměnných */
    processed_variables = tracked_calloc(general_vars->num_general_vars, sizeof(int));
    if (!processed_variables) {
        free_bounds(*bounds);
        return MEMORY_ERROR;
    }

    /* parsování řádků v sekci Bounds */
    for (j = 0; j < num_lines; j++) {
        token_count = 0;
        for (i = 0; i < MAX_TOKENS; i++) {
            tokens[i] = NULL;
        }

        /* odstranění bílých znaků */
        line = trim_white_space(lines[j]);

        /* kontrola, zda je proměnná neomezená */
        if (strstr(line, "free")) {
            sscanf(line, "%s free", var_name);

            lower_bound = -INFINITY;
            upper_bound = INFINITY;
        } else {
            /* proměnná má explicitně zadané meze */
            ptr = remove_spaces(line);

            /* kontrola validního zápisu */
            if (validate_expression(ptr)) {
                /*printf("ptr: %s\n", ptr);*/
                tracked_free(processed_variables);
                return SYNTAX_ERROR;
            }

            while (*ptr != '\0') {
                /* hledání operátorů v řetězci */
                operator_position = strpbrk(ptr, "<>");

                if (operator_position != NULL) {
                    /* extrahování podřetězce před operátorem */
                    if (operator_position > ptr) {
                        length = operator_position - ptr;

                        tokens[token_count] = tracked_malloc(length + 1);
                        if (!tokens[token_count]) {
                            fprintf(stderr, "Memory allocation failed.\n");
                            tracked_free(processed_variables);
                            return MEMORY_ERROR;
                        }

                        strncpy(tokens[token_count], ptr, length);
                        tokens[token_count][length] = '\0';
                        token_count++;
                    }

                    /* kontrola dvojznakových operátorů <= nebo >= */
                    if (*(operator_position + 1) == '=') {
                        tokens[token_count] = tracked_malloc(3);

                        if (!tokens[token_count]) {
                            fprintf(stderr, "Memory allocation failed.\n");
                            tracked_free(processed_variables);
                            return MEMORY_ERROR;
                        }

                        strncpy(tokens[token_count], operator_position, 2);
                        tokens[token_count][2] = '\0';
                        token_count++;
                        ptr = operator_position + 2;
                    } else {
                        /* operátor je pouze < nebo > */
                        tokens[token_count] = tracked_malloc(2);

                        if (!tokens[token_count]) {
                            fprintf(stderr, "Memory allocation failed.\n");
                            tracked_free(processed_variables);
                            return MEMORY_ERROR;
                        }

                        strncpy(tokens[token_count], operator_position, 1);
                        tokens[token_count][1] = '\0';
                        token_count++;
                        ptr = operator_position + 1;
                    }
                } else {
                    /* zpracování posledního tokenu */
                    tokens[token_count] = tracked_malloc(strlen(ptr) + 1);

                    if (!tokens[token_count]) {
                        fprintf(stderr, "Memory allocation failed.\n");
                        tracked_free(processed_variables);
                        return MEMORY_ERROR;
                    }

                    strcpy(tokens[token_count], ptr);
                    token_count++;
                    break;
                }
            }

            /* analyzování získaných tokenů */
            for (i = 0; i < token_count; ++i) {
                if (!is_number(tokens[i])) {
                    /* pokud není číslo, nastavují se hranice */
                    if (i + 1 < token_count) {
                        if (strcasecmp(tokens[i + 1], "<") == 0 || strcasecmp(tokens[i + 1], "<=") == 0) {
                            lower_bound = strtod(tokens[i], NULL);
                        } else if (strcasecmp(tokens[i + 1], ">") == 0 || strcasecmp(tokens[i + 1], ">=") == 0) {
                            upper_bound = strtod(tokens[i], NULL);
                        }
                    } else {
                        /* kontrola operátoru na konci */
                        if (strcasecmp(tokens[i - 1], "<") == 0 || strcasecmp(tokens[i - 1], "<=") == 0) {
                            upper_bound = strtod(tokens[i], NULL);
                        } else if (strcasecmp(tokens[i - 1], ">") == 0 || strcasecmp(tokens[i - 1], ">=") == 0) {
                            lower_bound = strtod(tokens[i], NULL);
                        }
                    }
                    /* zpracování hodnot nekonečno */
                } else if (strcasecmp(tokens[i], "inf") == 0 || strcasecmp(tokens[i], "-inf") == 0 ||
                           strcasecmp(tokens[i], "infinity") == 0 || strcasecmp(tokens[i], "-infinity") == 0) {
                    value = (strcasecmp(tokens[i], "inf") == 0 || strcasecmp(tokens[i], "infinity") == 0)
                                ? INFINITY
                                : -INFINITY;
                    if (i + 1 < token_count) {
                        if (strcasecmp(tokens[i + 1], "<") == 0 || strcasecmp(tokens[i + 1], "<=") == 0) {
                            lower_bound = value;
                        } else if (strcasecmp(tokens[i + 1], ">") == 0 || strcasecmp(tokens[i + 1], ">=") == 0) {
                            upper_bound = value;
                        }
                    } else {
                        if (strcasecmp(tokens[i - 1], "<") == 0 || strcasecmp(tokens[i - 1], "<=") == 0) {
                            upper_bound = value;
                        } else if (strcasecmp(tokens[i - 1], ">") == 0 || strcasecmp(tokens[i - 1], ">=") == 0) {
                            lower_bound = value;
                        }
                    }
                } else {
                    /* uložení názvu proměnné */
                    if (!(strcasecmp(tokens[i], "<") == 0 || strcasecmp(tokens[i], "<=") == 0 ||
                          strcasecmp(tokens[i], ">") == 0 || strcasecmp(tokens[i], ">=") == 0)) {
                        strncpy(var_name, tokens[i], sizeof(var_name) - 1);
                        var_name[sizeof(var_name) - 1] = '\0';
                    }
                }
            }
        }
        /*
                printf("Varname: %s\n", var_name);
                printf("Lower bound: %f\n", lower_bound);
                printf("Upper bound: %f\n", upper_bound);
        */

        /* kontrola, zda je proměnná známá */
        if ((result_code = is_var_known(general_vars, var_name))) {
            tracked_free(processed_variables);
            return result_code;
        }

        /* získání indexu proměnné z pole v general_vars a nastavení hranic */
        var_index = get_var_index(general_vars, var_name);
        processed_variables[var_index] = 1;

        add_bound(*bounds, lower_bound, upper_bound, var_index);

        /* uvolnění tokenů */
        for (i = 0; i < token_count; ++i) {
            tracked_free(tokens[i]);
        }
    }

    /* přidání výchozích hranic nezpracovaným proměnným */
    for (i = 0; i < general_vars->num_general_vars; i++) {
        if (!processed_variables[i]) {
            add_bound(*bounds, DEFAULT_LOWER_BOUND, DEFAULT_UPPER_BOUND, i);
        }
    }

    tracked_free(processed_variables);
    return 0;
}

int is_number(char *str) {
    /* sanity check */
    if (!str) {
        return 1;
    }

    if (*str == '-' || *str == '+') {
        str++;
    }

    while (*str) {
        if (!isdigit(*str) && *str != '.') {
            return 1;
        }
        str++;
    }

    return 0;
}
