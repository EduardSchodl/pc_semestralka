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

    printf("Freeing bounds\n");

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

    bounds->var_names[bounds->num_vars] = strdup(var_name);
    if (!bounds->var_names[bounds->num_vars]) {
        return;
    }

    bounds->lower_bound[bounds->num_vars] = lower_bound;
    bounds->upper_bound[bounds->num_vars] = upper_bound;
    bounds->num_vars++;
}

/*
 * zkontrolvat, zda řádka obsahuje <,>,<=,>= a nevalidní operátory
 */
int parse_bounds(Bounds *bounds, char *line) {
    double lower_bound = 0;
    double upper_bound = INFINITY;
    char var_name[50] = {0};
    char *ptr = line;
    char *lowerptr, *upperptr;

    /* sanity check */
    if(!line) {
        return 93;
    }

    /* var is unbounded */
    if (strstr(ptr, "free")) {
        if(contains_invalid_operator_sequence(line)) {
            return 11;
        }

        sscanf(ptr, "%s free", var_name);
        lower_bound = NEGATIVE_INFINITY;
        upper_bound = INFINITY;
    }
    /* starts with digit */
    else if (isdigit(*ptr) || *ptr == '-') {
        remove_spaces(ptr);

        if(bounds_valid_operators(line)) {
            return 11;
        }

        lowerptr = strstr(ptr, "<=");
        if (!lowerptr) {
            lowerptr = strstr(ptr, "<");
        }

        if (lowerptr) {
            lower_bound = strtod(ptr, NULL);
            ptr = lowerptr + (lowerptr[1] == '=' ? 2 : 1);

            while (*ptr && *ptr != '<') {
                strncat(var_name, ptr, 1);
                ptr++;
            }

            upperptr = strstr(ptr, "<=");
            if (!upperptr) {
                upperptr = strstr(ptr, "<");
            }

            if (upperptr) {
                ptr = upperptr + (upperptr[1] == '=' ? 2 : 1);
                upper_bound = strtod(ptr, NULL);
            }
        }
        else {
            upperptr = strstr(ptr, ">=");
            if (!upperptr) {
                upperptr = strstr(ptr, ">");
            }

            upper_bound = strtod(ptr, NULL);
            ptr = upperptr + (upperptr[1] == '=' ? 2 : 1);

            while (*ptr && *ptr != '>') {
                strncat(var_name, ptr, 1);
                ptr++;
            }

            lowerptr = strstr(ptr, ">=");
            if (!lowerptr) {
                lowerptr = strstr(ptr, ">");
            }

            if (lowerptr) {
                ptr = lowerptr + (lowerptr[1] == '=' ? 2 : 1);
                lower_bound = strtod(ptr, NULL);
            }
        }
    }
    /* starts with variable */
    else {
        remove_spaces(ptr);

        if(bounds_valid_operators(line)) {
            printf("adsasdasdasds");
            return 11;
        }

        while (*ptr && *ptr != '<' && *ptr != '>') {
            strncat(var_name, ptr, 1);
            ptr++;
        }

        if (*ptr == '<') {
            ptr += (*(ptr + 1) == '=' ? 2 : 1);
            upper_bound = strtod(ptr, NULL);
        } else if (*ptr == '>') {
            ptr += (*(ptr + 1) == '=' ? 2 : 1);
            lower_bound = strtod(ptr, NULL);
        }
    }

    if(is_valid_string(var_name)) {
        printf("invalidni varname '%s'\n", var_name);
        return 11;
    }

    add_bound(bounds, var_name, lower_bound, upper_bound);

    return 1;
}