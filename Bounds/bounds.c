#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bounds.h"
#include "../parse.h"
#include "../validate.h"

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

int parse_bounds(Bounds **bounds, General_vars *general_vars, char **lines, int num_lines) {
    char *tokens[255];
    int token_count = 0;
    char *ptr;
    char *op_pos;
    int i, j, res_code;
    double value;
    double lower_bound = 0;
    double upper_bound = INFINITY;
    char var_name[50] = {0};
    char *line = NULL;

    if (!lines || !num_lines) {
        return 93;
    }

    *bounds = create_bounds(INITIAL_SIZE);
    if(!*bounds) {
        return 93;
    }

    for (j = 0; j < num_lines; j++) {
        token_count = 0;
        for (i = 0; i < 255; i++) {
            tokens[i] = NULL;
        }

        line = trim_white_space(lines[j]);

        if (strstr(line, "free")) {
            sscanf(line, "%s free", var_name);

            lower_bound = -INFINITY;
            upper_bound = INFINITY;
        } else {
            ptr = remove_spaces(line);

            while (*ptr != '\0') {
                op_pos = strpbrk(ptr, "<>");

                if (op_pos != NULL) {
                    if (op_pos > ptr) {
                        int length = op_pos - ptr;

                        tokens[token_count] = malloc(length + 1);
                        if (!tokens[token_count]) {
                            fprintf(stderr, "Memory allocation failed.\n");
                            return 93;
                        }

                        strncpy(tokens[token_count], ptr, length);
                        tokens[token_count][length] = '\0';
                        token_count++;
                    }

                    if (*(op_pos + 1) == '=') {
                        tokens[token_count] = malloc(3);
                        if (!tokens[token_count]) {
                            fprintf(stderr, "Memory allocation failed.\n");
                            return 93;
                        }

                        strncpy(tokens[token_count], op_pos, 2);
                        tokens[token_count][2] = '\0';
                        token_count++;
                        ptr = op_pos + 2;
                    } else {
                        tokens[token_count] = malloc(2);
                        if (!tokens[token_count]) {
                            fprintf(stderr, "Memory allocation failed.\n");
                            return 93;
                        }

                        strncpy(tokens[token_count], op_pos, 1);
                        tokens[token_count][1] = '\0';
                        token_count++;
                        ptr = op_pos + 1;
                    }
                } else {
                    tokens[token_count] = malloc(strlen(ptr) + 1);
                    if (!tokens[token_count]) {
                        fprintf(stderr, "Memory allocation failed.\n");
                        return 93;
                    }

                    strcpy(tokens[token_count], ptr);
                    token_count++;
                    break;
                }
            }
            /*
                    for (i = 0; i < token_count; i++) {
                        printf("Token %d: %s\n", i + 1, tokens[i]);
                    }
            */
            for (i = 0; i < token_count; ++i) {
                if (is_number(tokens[i])) {
                    if (i + 1 < token_count) {
                        if (strcmp(tokens[i + 1], "<") == 0 || strcmp(tokens[i + 1], "<=") == 0) {
                            lower_bound = strtod(tokens[i], NULL);
                        } else if (strcmp(tokens[i + 1], ">") == 0 || strcmp(tokens[i + 1], ">=") == 0) {
                            upper_bound = strtod(tokens[i], NULL);
                        }
                    } else {
                        if (strcmp(tokens[i - 1], "<") == 0 || strcmp(tokens[i - 1], "<=") == 0) {
                            upper_bound = strtod(tokens[i], NULL);
                        } else if (strcmp(tokens[i - 1], ">") == 0 || strcmp(tokens[i - 1], ">=") == 0) {
                            lower_bound = strtod(tokens[i], NULL);
                        }
                    }
                } else if (strcasecmp(tokens[i], "inf") == 0 || strcasecmp(tokens[i], "-inf") == 0) {
                    value = (strcasecmp(tokens[i], "inf") == 0) ? INFINITY : -INFINITY;
                    if (i + 1 < token_count) {
                        if (strcmp(tokens[i + 1], "<") == 0 || strcmp(tokens[i + 1], "<=") == 0) {
                            lower_bound = value;
                        } else if (strcmp(tokens[i + 1], ">") == 0 || strcmp(tokens[i + 1], ">=") == 0) {
                            upper_bound = value;
                        }
                    } else {
                        if (strcmp(tokens[i - 1], "<") == 0 || strcmp(tokens[i - 1], "<=") == 0) {
                            upper_bound = value;
                        } else if (strcmp(tokens[i - 1], ">") == 0 || strcmp(tokens[i - 1], ">=") == 0) {
                            lower_bound = value;
                        }
                    }
                } else {
                    if (!(strcmp(tokens[i], "<") == 0 || strcmp(tokens[i], "<=") == 0 || strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], ">=") == 0)) {
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
        if ((res_code = is_var_known(general_vars, var_name))) {
            return res_code;
        }

        add_bound(*bounds, var_name, lower_bound, upper_bound);

        for (i = 0; i < token_count; ++i) {
            free(tokens[i]);
        }
    }

    return 0;
}

int is_number(char *str) {
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