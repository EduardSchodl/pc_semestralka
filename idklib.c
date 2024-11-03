#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "idklib.h"

void read_input_file(FILE *file){
    int i;
    int initial_value = 2;
    char line[256];
    int isObjective = 0;
    int isConstraints = 0;
    int isBounds = 0;
    int isGenerals = 0;

    GeneralVars *general_vars;
    char *var;

    /* sanity check */
    if(!file) {
        return;
    }

    /* allocate GeneralVars */
    general_vars = create_general_vars(initial_value);
    if(!general_vars) {
        return;
    }

    general_vars->num_general_vars = 0;

    printf("\n");

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Maximize", 8) == 0 || strncmp(line, "Minimize", 8) == 0) {
            isObjective = 1;

            isConstraints = 0;
            isBounds = 0;
            isGenerals = 0;
            continue;
        } else if (strncmp(line, "Subject To", 10) == 0) {
            isConstraints = 1;

            isObjective = 0;
            isBounds = 0;
            isGenerals = 0;
            continue;
        } else if (strncmp(line, "Generals", 8) == 0) {
            isGenerals = 1;

            isObjective = 0;
            isConstraints = 0;
            isBounds = 0;
            continue;
        } else if (strncmp(line, "Bounds", 6) == 0) {
            isBounds = 1;

            isObjective = 0;
            isConstraints = 0;
            isGenerals = 0;
            continue;
        } else if (strncmp(line, "End", 3) == 0) {
            break;
        }

        if (isObjective) {
            printf("Objective: %s", line);
        } else if (isConstraints) {
            printf("Constraint: %s", line);
        } else if (isBounds) {
            printf("Bound: %s", line);

        } else if (isGenerals) {
            printf("Generals: %s", line);
            var = strtok(line, " ");

            while (var != NULL) {
                /* printf("Token: %s\n", var); */
                add_variable(general_vars, var);
                var = strtok(NULL, " ");
            }
        }
    }
    printf("\n");

    for(i = 0; i < general_vars->num_general_vars; i++) {
        printf("Variable: %s\n", general_vars->general_vars[i]);
    }

    free_general_vars(general_vars);
}

void add_variable(GeneralVars *gv, char *var_name) {
    int i;
    int new_size;
    char **new_general_vars;
    Bounds *new_bounds;

    if(!gv) {
        return;
    }

    if (gv->num_general_vars >= gv->max_vars) {
        new_size = gv->max_vars * 2;
        new_general_vars = realloc(gv->general_vars, new_size * sizeof(char*));
        if (!new_general_vars) {
            return;
        }
        gv->general_vars = new_general_vars;

        new_bounds = realloc(gv->bounds, new_size * sizeof(Bounds));
        if (!new_bounds) {
            return;
        }
        gv->bounds = new_bounds;

        for (i = gv->max_vars; i < new_size; i++) {
            gv->bounds[i].lower_bound = 0;
            gv->bounds[i].upper_bound = INFINITY;
        }

        gv->max_vars = new_size;
    }

    gv->general_vars[gv->num_general_vars] = malloc((strlen(var_name) + 1) * sizeof(char));
    if (!gv->general_vars[gv->num_general_vars]) {
        return;
    }
    strcpy(gv->general_vars[gv->num_general_vars], var_name);
    gv->num_general_vars++;
}

GeneralVars* create_general_vars(int initial_size) {
    int i;
    GeneralVars *gv = malloc(sizeof(GeneralVars));
    if(!gv) {
        return NULL;
    }
    gv->general_vars = malloc(initial_size * sizeof(char*));
    if(!gv->general_vars) {
        free(gv);
        return NULL;
    }
    gv->bounds = malloc(initial_size * sizeof(Bounds));
    if(!gv->bounds) {
        free(gv->general_vars);
        free(gv);
        return NULL;
    }

    gv->num_general_vars = 0;
    gv->max_vars = initial_size;

    for (i = 0; i < gv->max_vars; i++) {
        gv->bounds[i].lower_bound = 0;
        gv->bounds[i].upper_bound = INFINITY;
    }

    return gv;
}

void free_general_vars(GeneralVars *gv) {
    int i;
    for (i = 0; i < gv->num_general_vars; i++) {
        free(gv->general_vars[i]);
    }
    free(gv->general_vars);
    free(gv->bounds);
    free(gv);
}

void solve_linear_programming(){

}

void print_solution(){

}