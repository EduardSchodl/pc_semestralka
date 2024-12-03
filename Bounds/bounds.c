#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bounds.h"
#include "../Parse//parse.h"
#include "../Validate//validate.h"
#include "../Memory_manager/memory_manager.h"

#define DEFAULT_LOWER 0.0
#define DEFAULT_UPPER INFINITY

Bounds *create_bounds(const int initial_size) {
    int i;
    Bounds *temp;

    if (!initial_size) {
        return NULL;
    }

    temp = (Bounds *)tracked_malloc(sizeof(Bounds));
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
        temp->lower_bound[i] = DEFAULT_LOWER;
        temp->upper_bound[i] = DEFAULT_UPPER;
    }

    temp->num_vars = 0;
    temp->max_vars = initial_size;

    return temp;
}

void free_bounds(Bounds *bounds) {
    if (bounds) {
        tracked_free(bounds->lower_bound);
        tracked_free(bounds->upper_bound);
        tracked_free(bounds);
    }
}

void add_bound(Bounds *bounds, const double lower_bound, const double upper_bound, int var_index) {
    int new_size, i;
    double *new_lower_bound, *new_upper_bound;

    if (!bounds) {
        return;
    }

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
            bounds->lower_bound[i] = DEFAULT_LOWER;
            bounds->upper_bound[i] = DEFAULT_UPPER;
        }

        bounds->max_vars = new_size;
    }

    bounds->lower_bound[var_index] = lower_bound;
    bounds->upper_bound[var_index] = upper_bound;

    if (var_index >= bounds->num_vars) {
        bounds->num_vars = var_index + 1;
    }
}

int parse_bounds(Bounds **bounds, General_vars *general_vars, char **lines, int num_lines) {
    char *tokens[255];
    int token_count = 0;
    char *ptr;
    char *op_pos;
    int i, j, res_code;
    double value;
    double lower_bound = DEFAULT_LOWER;
    double upper_bound = DEFAULT_UPPER;
    char var_name[50] = {0};
    char *line = NULL;
    int *processed_flags;
    int var_index;

    if (!lines || !general_vars) {
        return 93;
    }

    *bounds = create_bounds(general_vars->num_general_vars);
    if(!*bounds) {
        return 93;
    }

    processed_flags = tracked_calloc(general_vars->num_general_vars, sizeof(int));
    if (!processed_flags) {
        free_bounds(*bounds);
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

                        tokens[token_count] = tracked_malloc(length + 1);
                        if (!tokens[token_count]) {
                            fprintf(stderr, "Memory allocation failed.\n");
                            tracked_free(processed_flags);
                            return 93;
                        }

                        strncpy(tokens[token_count], ptr, length);
                        tokens[token_count][length] = '\0';
                        token_count++;
                    }

                    if (*(op_pos + 1) == '=') {
                        tokens[token_count] = tracked_malloc(3);
                        if (!tokens[token_count]) {
                            fprintf(stderr, "Memory allocation failed.\n");
                            tracked_free(processed_flags);
                            return 93;
                        }

                        strncpy(tokens[token_count], op_pos, 2);
                        tokens[token_count][2] = '\0';
                        token_count++;
                        ptr = op_pos + 2;
                    } else {
                        tokens[token_count] = tracked_malloc(2);
                        if (!tokens[token_count]) {
                            fprintf(stderr, "Memory allocation failed.\n");
                            tracked_free(processed_flags);
                            return 93;
                        }

                        strncpy(tokens[token_count], op_pos, 1);
                        tokens[token_count][1] = '\0';
                        token_count++;
                        ptr = op_pos + 1;
                    }
                } else {
                    tokens[token_count] = tracked_malloc(strlen(ptr) + 1);
                    if (!tokens[token_count]) {
                        fprintf(stderr, "Memory allocation failed.\n");
                        tracked_free(processed_flags);
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
                if (!is_number(tokens[i])) {
                    if (i + 1 < token_count) {
                        if (strcasecmp(tokens[i + 1], "<") == 0 || strcasecmp(tokens[i + 1], "<=") == 0) {
                            lower_bound = strtod(tokens[i], NULL);
                        } else if (strcasecmp(tokens[i + 1], ">") == 0 || strcasecmp(tokens[i + 1], ">=") == 0) {
                            upper_bound = strtod(tokens[i], NULL);
                        }
                    } else {
                        if (strcasecmp(tokens[i - 1], "<") == 0 || strcasecmp(tokens[i - 1], "<=") == 0) {
                            upper_bound = strtod(tokens[i], NULL);
                        } else if (strcasecmp(tokens[i - 1], ">") == 0 || strcasecmp(tokens[i - 1], ">=") == 0) {
                            lower_bound = strtod(tokens[i], NULL);
                        }
                    }
                } else if (strcasecmp(tokens[i], "inf") == 0 || strcasecmp(tokens[i], "-inf") == 0 || strcasecmp(tokens[i], "infinity") == 0 || strcasecmp(tokens[i], "-infinity") == 0) {
                    value = (strcasecmp(tokens[i], "inf") == 0 || strcasecmp(tokens[i], "infinity") == 0) ? INFINITY : -INFINITY;
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
                    if (!(strcasecmp(tokens[i], "<") == 0 || strcasecmp(tokens[i], "<=") == 0 || strcasecmp(tokens[i], ">") == 0 || strcasecmp(tokens[i], ">=") == 0)) {
                        strncpy(var_name, tokens[i], sizeof(var_name) - 1);
                        var_name[sizeof(var_name) - 1] = '\0';
                    }
                }
            }
        }

        printf("Varname: %s\n", var_name);
        printf("Lower bound: %f\n", lower_bound);
        printf("Upper bound: %f\n", upper_bound);

        if ((res_code = is_var_known(general_vars, var_name))) {
            tracked_free(processed_flags);
            return res_code;
        }

        var_index = get_var_index(general_vars, var_name);
        processed_flags[var_index] = 1;

        add_bound(*bounds, lower_bound, upper_bound, var_index);

        for (i = 0; i < token_count; ++i) {
            tracked_free(tokens[i]);
        }
    }

    for (i = 0; i < general_vars->num_general_vars; i++) {
        if (!processed_flags[i]) {
            add_bound(*bounds, DEFAULT_LOWER, DEFAULT_UPPER, i);
        }
    }

    tracked_free(processed_flags);
    return 0;
}

int is_number(char *str) {
    if(!str) {
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