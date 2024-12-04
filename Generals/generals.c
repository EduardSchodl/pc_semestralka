#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "generals.h"
#include "../Parse/parse.h"
#include "../Validate/validate.h"
#include "../Memory_manager/memory_manager.h"

int parse_generals(General_vars **general_vars, char **lines, const int num_lines) {
    const char *start;
    const char *end;
    char buffer[256];
    size_t length;
    int i;

    if(!lines || !num_lines || !*lines) {
        return 93;
    }

    *general_vars = create_general_vars(INITIAL_SIZE);
    if(!*general_vars) {
        return 93;
    }

    for (i = 0; i < num_lines; i++) {
        start = lines[i];

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

            length = end - start;

            if (length >= sizeof(buffer)) {
                printf("Error: word too long for buffer.\n");
                return 12;
            }

            strncpy(buffer, start, length);
            buffer[length] = '\0';

            trim_white_space(buffer);

            if (is_valid_string(buffer)) {
                return 11;
            }

            add_variable(*general_vars, buffer);

            start = end;
        }
    }

    return 0;
}

int get_var_index(General_vars *general_vars, char *var_name) {
    int i;

    if(!general_vars || !var_name) {
        return -1;
    }

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
    int *new_used_vars;

    if (!gv || !var_name) {
        return;
    }

    if (gv->num_general_vars >= gv->max_vars) {
        new_size = gv->max_vars + 10;

        new_general_vars = tracked_realloc(gv->general_vars, new_size * sizeof(char *));
        if (!new_general_vars) {
            return;
        }
        gv->general_vars = new_general_vars;

        new_used_vars = tracked_realloc(gv->used_vars, new_size * sizeof(int));
        if (!new_used_vars) {
            return;
        }
        gv->used_vars = new_used_vars;

        gv->max_vars = new_size;
    }

    gv->general_vars[gv->num_general_vars] = tracked_malloc((strlen(var_name) + 1) * sizeof(char));
    if (!gv->general_vars[gv->num_general_vars]) {
        return;
    }
    strcpy(gv->general_vars[gv->num_general_vars], var_name);

    gv->used_vars[gv->num_general_vars] = 0;

    gv->num_general_vars++;
}

General_vars* create_general_vars(const int initial_size) {
    General_vars *temp;
    int i;

    if(!initial_size) {
        return NULL;
    }

    temp = tracked_malloc(sizeof(General_vars));
    if(!temp) {
        return NULL;
    }
    temp->general_vars = tracked_malloc(initial_size * sizeof(char*));
    if(!temp->general_vars) {
        tracked_free(temp);
        return NULL;
    }

    temp->used_vars = tracked_malloc(initial_size * sizeof(int));
    if(!temp->used_vars) {
        for (i = 0; i < initial_size; i++) {
            tracked_free(temp->general_vars[i]);
        }
        tracked_free(temp->general_vars);
        tracked_free(temp);
        return NULL;
    }

    for (i = 0; i < initial_size; i++) {
        temp->used_vars[i] = 0;
    }

    temp->num_general_vars = 0;
    temp->max_vars = initial_size;

    return temp;
}

void free_general_vars(General_vars *vars) {
    int i;

    if (!vars) {
        return;
    }

    if (vars->general_vars) {
        for (i = 0; i < vars->num_general_vars; i++) {
            tracked_free(vars->general_vars[i]);
        }
        tracked_free(vars->general_vars);
    }

    tracked_free(vars->used_vars);

    tracked_free(vars);
}