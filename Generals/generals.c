#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generals.h"

#include <ctype.h>

#include "../parse.h"
#include "../validate.h"

int parse_generals(General_vars *general_vars, char *line) {
    char *start = line;
    char *end;
    while (*start) {
        while (*start && isspace(*start)) {
            start++;
        }

        if (*start == '\0') {
            break;
        }

        end = start;
        while (*end && !isspace(*end)) {
            end++;
        }

        *end = '\0';
        if (is_valid_string(trim_white_space(start))) {
            return 11;
        }
        add_variable(general_vars, trim_white_space(start));

        start = end + 1;
    }

    return 0;
}

int get_var_index(General_vars *general_vars, char *var_name) {
    int i;

    for (i = 0; i < general_vars->num_general_vars; i++) {
        if (strcmp(general_vars->general_vars[i], var_name) == 0) {
            return i;
        }
    }

    return -1;
}

void add_variable(General_vars *gv, const char *var_name) {
    int new_size;
    char **new_general_vars;

    if(!gv) {
        return;
    }

    if (gv->num_general_vars >= gv->max_vars) {
        new_size = gv->max_vars + 10;
        new_general_vars = realloc(gv->general_vars, new_size * sizeof(char*));
        if (!new_general_vars) {
            return;
        }
        gv->general_vars = new_general_vars;

        gv->max_vars = new_size;
    }

    gv->general_vars[gv->num_general_vars] = malloc((strlen(var_name) + 1) * sizeof(char));
    if (!gv->general_vars[gv->num_general_vars]) {
        return;
    }
    strcpy(gv->general_vars[gv->num_general_vars], var_name);
    gv->num_general_vars++;
}

General_vars* create_general_vars(const int initial_size) {
    General_vars *temp;

    temp = malloc(sizeof(General_vars));
    if(!temp) {
        return NULL;
    }
    temp->general_vars = malloc(initial_size * sizeof(char*));
    if(!temp->general_vars) {
        free(temp);
        return NULL;
    }

    temp->num_general_vars = 0;
    temp->max_vars = initial_size;

    return temp;
}

void free_general_vars(General_vars *general_vars) {
    int i;

    printf("Freeing general vars\n");

    for (i = 0; i < general_vars->num_general_vars; i++) {
        free(general_vars->general_vars[i]);
    }
    free(general_vars->general_vars);
    free(general_vars);
}