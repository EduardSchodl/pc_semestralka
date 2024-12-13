#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "generals.h"
#include "../Parse/parse.h"
#include "../Validate/validate.h"
#include "../Memory_manager/memory_manager.h"

int parse_generals(General_vars **general_vars, char **lines, const int num_lines) {
    char buffer[256], *token;
    int i;

    /* sanity check */
    if(!lines || !num_lines || !*lines) {
        return 93;
    }

    /* vytvoření struktury General_vars */
    *general_vars = create_general_vars(INITIAL_SIZE);
    if(!*general_vars) {
        return 93;
    }

    /* zpracování jednotlivých řádek sekce Generals */
    for (i = 0; i < num_lines; i++) {
        /* tokenizace proměnných v řádce */
        token = strtok(lines[i], " \n");

        while(token) {
            strncpy(buffer, token, sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';

            trim_white_space(buffer);

            /* kontrola validity názvu proměnné */
            if (is_valid_string(buffer)) {
                return 11;
            }

            /* přidání proměnné do pole */
            add_variable(*general_vars, buffer);

            token = strtok(NULL, " \n");
        }
    }

    return 0;
}

int get_var_index(General_vars *general_vars, char *var_name) {
    int i;

    /* sanity check */
    if(!general_vars || !var_name) {
        return -1;
    }

    /* získání indexu proměnné v poli general_vars */
    for (i = 0; i < general_vars->num_general_vars; i++) {
        if (strcmp(general_vars->general_vars[i], var_name) == 0) {
            return i;
        }
    }

    return -1;
}

void add_variable(General_vars *general_vars, const char *var_name) {
    int new_size;
    char **new_general_vars;
    int *new_used_vars;

    /* sanity check */
    if (!general_vars || !var_name) {
        return;
    }

    /* pokud počet proměnných přesáhne hranici, zvětší se a realokuje pole */
    if (general_vars->num_general_vars >= general_vars->max_vars) {
        new_size = general_vars->max_vars + 10;

        new_general_vars = tracked_realloc(general_vars->general_vars, new_size * sizeof(char *));
        if (!new_general_vars) {
            return;
        }
        general_vars->general_vars = new_general_vars;

        new_used_vars = tracked_realloc(general_vars->used_vars, new_size * sizeof(int));
        if (!new_used_vars) {
            return;
        }
        general_vars->used_vars = new_used_vars;

        general_vars->max_vars = new_size;
    }

    general_vars->general_vars[general_vars->num_general_vars] = tracked_malloc((strlen(var_name) + 1) * sizeof(char));
    if (!general_vars->general_vars[general_vars->num_general_vars]) {
        return;
    }

    /* přidání promměné */
    strcpy(general_vars->general_vars[general_vars->num_general_vars], var_name);

    /* nastavení příznaku využití */
    general_vars->used_vars[general_vars->num_general_vars] = 0;

    general_vars->num_general_vars++;
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

void free_general_vars(General_vars *general_vars) {
    int i;

    /* sanity check */
    if (!general_vars) {
        return;
    }

    /* uvolnění struktury General_vars */
    if (general_vars->general_vars) {
        for (i = 0; i < general_vars->num_general_vars; i++) {
            tracked_free(general_vars->general_vars[i]);
        }
        tracked_free(general_vars->general_vars);
    }

    tracked_free(general_vars->used_vars);

    tracked_free(general_vars);
}