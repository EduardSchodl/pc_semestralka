#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bounds.h"
#include "../validate.h"
#include "../parse.h"

Bounds *create_bounds(const int initial_size) {
    int i;
    Bounds *temp;

    temp = (Bounds *)malloc(sizeof(Bounds));
    if (!temp) {
        return NULL;
    }

    temp->var_names = malloc(initial_size * sizeof(char *));
    if (!temp->var_names) {
        free(temp);
        return NULL;
    }

    temp->lower_bound = malloc(initial_size * sizeof(double));
    if (!temp->lower_bound) {
        free(temp->var_names);
        free(temp);
        return NULL;
    }

    temp->upper_bound = malloc(initial_size * sizeof(double));
    if (!temp->upper_bound) {
        free(temp->lower_bound);
        free(temp->var_names);
        free(temp);
        return NULL;
    }

    for (i = 0; i < initial_size; i++) {
        temp->lower_bound[i] = 0.0;
        temp->upper_bound[i] = INFINITY;
    }

    temp->num_vars = 0;
    temp->max_vars = initial_size;

    return temp;
}

void free_bounds(Bounds *bounds) {
    int i;

    if (bounds) {
        if (bounds->var_names) {
            for (i = 0; i < bounds->num_vars; i++) {
                free(bounds->var_names[i]);
            }
            free(bounds->var_names);
        }
        free(bounds->lower_bound);
        free(bounds->upper_bound);
        free(bounds);
    }
}

void add_bound(Bounds *bounds, const char *var_name, const double lower_bound, const double upper_bound) {
    int new_size;
    char **new_var_names;
    double *new_lower_bound, *new_upper_bound;

    if (!bounds) {
        return;
    }

    if (bounds->num_vars >= bounds->max_vars) {
        new_size = bounds->max_vars + 10;

        new_var_names = realloc(bounds->var_names, new_size * sizeof(char *));
        if (!new_var_names) {
            return;
        }
        bounds->var_names = new_var_names;

        new_lower_bound = realloc(bounds->lower_bound, new_size * sizeof(double));
        if (!new_lower_bound) {
            return;
        }
        bounds->lower_bound = new_lower_bound;

        new_upper_bound = realloc(bounds->upper_bound, new_size * sizeof(double));
        if (!new_upper_bound) {
            return;
        }
        bounds->upper_bound = new_upper_bound;

        bounds->max_vars = new_size;
    }

    bounds->var_names[bounds->num_vars] = malloc(strlen(var_name) + 1);
    if (!bounds->var_names[bounds->num_vars]) {
        return;
    }

    strcpy(bounds->var_names[bounds->num_vars], var_name);

    bounds->lower_bound[bounds->num_vars] = lower_bound;
    bounds->upper_bound[bounds->num_vars] = upper_bound;
    bounds->num_vars++;
}


/*
 * zkontrolvat, zda řádka obsahuje <,>,<=,>= a nevalidní operátory
 */
int parse_bounds(Bounds *bounds, char *line) {
    char *operators[] = { "<=", ">=", "<", ">" };
    char *tokens[255];
    int token_count = 0;
    char *ptr;
    char *next_ptr;
    int i;
    char *op_pos;
    int op_length = 0;
    int var_index = -1;

    double lower_bound = 0;
    double upper_bound = INFINITY;
    char var_name[50] = {0};

    if (strstr(line, "free")) {
        sscanf(line, "%s free", var_name);

        lower_bound = NEGATIVE_INFINITY;
        upper_bound = INFINITY;
    }
    else {
        ptr = remove_spaces(line);

        /* Tokenize based on operators */
        while (*ptr != '\0') {
            /* Find the next operator */
            next_ptr = NULL;

            for (i = 0; i < 4; ++i) {
                op_pos = strstr(ptr, operators[i]);
                if (op_pos && (!next_ptr || op_pos < next_ptr)) {
                    next_ptr = op_pos;
                    op_length = strlen(operators[i]);
                }
            }

            /* If an operator was found, split */
            if (next_ptr) {
                /* Extract the part before the operator */
                if (next_ptr != ptr) {
                    tokens[token_count] = (char *)malloc(next_ptr - ptr + 1);
                    strncpy(tokens[token_count], ptr, next_ptr - ptr);
                    tokens[token_count][next_ptr - ptr] = '\0';  /* Null-terminate the string */
                    token_count++;
                }

                /* Extract the operator */
                tokens[token_count] = (char *)malloc(op_length + 1);
                strncpy(tokens[token_count], next_ptr, op_length);
                tokens[token_count][op_length] = '\0';  /* Null-terminate the string */
                token_count++;

                /* Move the pointer after the operator */
                ptr = next_ptr + op_length;
            } else {
                /* If no more operators, the rest is a single token */
                tokens[token_count] = strdup(ptr);  /* Duplicate the remaining string */
                token_count++;
                break;
            }
        }

        /* Print the tokens */
        /*
        printf("Tokens:\n");
        for (i = 0; i < token_count; ++i) {
            printf("Token %d: %s\n", i + 1, tokens[i]);
        }
        */

        /* Parse the tokens and assign bounds */
        for (i = 0; i < token_count; ++i) {
            if (is_number(tokens[i])) {
                /* If it's a number, determine which bound it belongs to */
                if (i + 1 < token_count) {
                    /* Check the next token for an operator */
                    if (strcmp(tokens[i + 1], "<") == 0 || strcmp(tokens[i + 1], "<=") == 0) {
                        /* Next operator is a lower bound */
                        lower_bound = atof(tokens[i]);
                    } else if (strcmp(tokens[i + 1], ">") == 0 || strcmp(tokens[i + 1], ">=") == 0) {
                        /* Next operator is an upper bound */
                        upper_bound = atof(tokens[i]);
                    }
                }
                else {
                    if (strcmp(tokens[i - 1], "<") == 0 || strcmp(tokens[i - 1], "<=") == 0) {
                        /* Next operator is a lower bound */
                        upper_bound = atof(tokens[i]);
                    } else if (strcmp(tokens[i - 1], ">") == 0 || strcmp(tokens[i - 1], ">=") == 0) {
                        /* Next operator is an upper bound */
                        lower_bound = atof(tokens[i]);
                    }
                }
            } else if (strcmp(tokens[i], "inf") == 0) {
                if (i + 1 < token_count) {
                    /* Check the next token for an operator */
                    if (strcmp(tokens[i + 1], "<") == 0 || strcmp(tokens[i + 1], "<=") == 0) {
                        /* Next operator is a lower bound */
                        lower_bound = INFINITY;
                    } else if (strcmp(tokens[i + 1], ">") == 0 || strcmp(tokens[i + 1], ">=") == 0) {
                        /* Next operator is an upper bound */
                        upper_bound = INFINITY;
                    }
                }
                else {
                    if (strcmp(tokens[i - 1], "<") == 0 || strcmp(tokens[i - 1], "<=") == 0) {
                        /* Next operator is a lower bound */
                        upper_bound = INFINITY;
                    } else if (strcmp(tokens[i - 1], ">") == 0 || strcmp(tokens[i - 1], ">=") == 0) {
                        /* Next operator is an upper bound */
                        lower_bound = INFINITY;
                    }
                }
            } else if (strcmp(tokens[i], "-inf") == 0) {
                if (i + 1 < token_count) {
                    /* Check the next token for an operator */
                    if (strcmp(tokens[i + 1], "<") == 0 || strcmp(tokens[i + 1], "<=") == 0) {
                        /* Next operator is a lower bound */
                        lower_bound = NEGATIVE_INFINITY;
                    } else if (strcmp(tokens[i + 1], ">") == 0 || strcmp(tokens[i + 1], ">=") == 0) {
                        /* Next operator is an upper bound */
                        upper_bound = NEGATIVE_INFINITY;
                    }
                }
                else {
                    if (strcmp(tokens[i - 1], "<") == 0 || strcmp(tokens[i - 1], "<=") == 0) {
                        /* Next operator is a lower bound */
                        upper_bound = NEGATIVE_INFINITY;
                    } else if (strcmp(tokens[i - 1], ">") == 0 || strcmp(tokens[i - 1], ">=") == 0) {
                        /* Next operator is an upper bound */
                        lower_bound = NEGATIVE_INFINITY;
                    }
                }
            } else {
                if (!(strcmp(tokens[i], "<") == 0 || strcmp(tokens[i], "<=") == 0 || strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], ">=") == 0)) {
                    sscanf(tokens[i], "%s", var_name);
                }
            }
        }
    }

    /* Print the variable and bounds */
    printf("Varname: %s\n", var_name);
    printf("Lower bounds: %f\n", lower_bound);
    printf("Upper bounds: %f\n", upper_bound);

    add_bound(bounds, var_name, lower_bound, upper_bound);

    /* Free memory allocated for tokens */
    for (i = 0; i < token_count; ++i) {
        free(tokens[i]);
    }

    return 1;
}

int is_number(const char *str) {
    if (*str == '-' || *str == '+') {
        str++;
    }

    while (*str) {
        if (!isdigit(*str) && *str != '.') {
            return 0;
        }
        str++;
    }

    return 1;
}