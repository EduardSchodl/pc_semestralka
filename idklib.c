#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "idklib.h"

#include <ctype.h>

void read_input_file(FILE *file){
    int initial_value = 2;
    char line[256];
    int isObjective = 0;
    int isConstraints = 0;
    int isBounds = 0;
    int isGenerals = 0;

    BoundsList *bounds_list;
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

    bounds_list = create_bounds_list(initial_value);
    if(!bounds_list) {
        free_general_vars(general_vars);
        return;
    }

    general_vars->num_general_vars = 0;

    printf("\n");

    while (fgets(line, sizeof(line), file)) {
        if(strstr(line, "//") != NULL) {
            continue;
        }

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
            bind_bounds(general_vars, bounds_list);
            break;
        }

        if (isObjective) {
            printf("Objective: %s", line);
        } else if (isConstraints) {
            printf("Constraint: %s", line);
        } else if (isBounds) {
            printf("Bound: %s", line);
            parse_bounds(bounds_list, line);
        } else if (isGenerals) {
            printf("Generals: %s", line);
            var = strtok(line, " ");

            while (var != NULL) {
                /* printf("Token: %s\n", var); */
                var = trim_white_space(var);
                add_variable(general_vars, var);
                var = strtok(NULL, " ");
            }
        }
    }
    printf("\n");

    /*
    for(i = 0; i < bounds_list->count; i++) {
        printf("Boundsssssssssss: %s | %.6f | %.6f | %d | %d\n", bounds_list->bounds_array[i]->var_name, bounds_list->bounds_array[i]->lower_bound, bounds_list->bounds_array[i]->upper_bound, bounds_list->capacity, bounds_list->count);
    }
    */

    free_general_vars(general_vars);
    free_bounds_list(bounds_list);
}

char *trim_white_space(char *str){
    char *end;

    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

void bind_bounds(GeneralVars *general_vars, BoundsList *bounds_list) {
    int i, j;
    int unknown_variable;

    for(i = 0; i < bounds_list->count; i++) {
        unknown_variable = 1;
        for(j = 0; j < general_vars->num_general_vars; j++) {
            if(strcmp(bounds_list->bounds_array[i]->var_name, general_vars->general_vars[j]) == 0) {
                unknown_variable = 0;
                general_vars->bounds[j].lower_bound = bounds_list->bounds_array[i]->lower_bound;
                general_vars->bounds[j].upper_bound = bounds_list->bounds_array[i]->upper_bound;
                break;
            }
        }

        if(unknown_variable) {
            printf("Unknown variable '%s'!\n", bounds_list->bounds_array[i]->var_name);
        }
    }

    /*
    for(i = 0; i < general_vars->num_general_vars; i++) {
        printf("%s lower: %.6f upper: %.6f\n", general_vars->general_vars[i], general_vars->bounds[i].lower_bound, general_vars->bounds[i].upper_bound);
    }
    */
}

void add_bound(BoundsList *bounds_list, Bounds *bound) {
    Bounds **new_bounds_list;
    int new_capacity;

    if (bounds_list->count >= bounds_list->capacity) {
        new_capacity = bounds_list->capacity * 2;

        new_bounds_list = realloc(bounds_list->bounds_array, new_capacity * sizeof(Bounds *));
        if (!new_bounds_list) {
            return;
        }

        bounds_list->bounds_array = new_bounds_list;
        bounds_list->capacity = new_capacity;
    }

    bounds_list->bounds_array[bounds_list->count++] = bound;
}

BoundsList* create_bounds_list(int initial_capacity) {
    BoundsList *list = malloc(sizeof(BoundsList));
    if (!list) return NULL;

    list->bounds_array = malloc(initial_capacity * sizeof(Bounds *));
    if (!list->bounds_array) {
        free(list);
        return NULL;
    }

    list->count = 0;
    list->capacity = initial_capacity;
    return list;
}

void free_bounds_list(BoundsList *bounds_list) {
    int i;
    for (i = 0; i < bounds_list->count; i++) {
        free(bounds_list->bounds_array[i]->var_name);
        free(bounds_list->bounds_array[i]);
    }

    free(bounds_list->bounds_array);
    free(bounds_list);
}

void parse_bounds(BoundsList *bounds_list, char *line) {
    Bounds *tempBounds;
    double temp = 0;
    double lower_bound = 0;
    double upper_bound = INFINITY;
    char var_name[256] = "";
    char *ptr = line;

    tempBounds = malloc(sizeof(Bounds));
    if (!tempBounds) {
        return;
    }

    while (isspace(*ptr)) ptr++;

    if (strstr(ptr, "free")) {
        sscanf(ptr, "%s free", var_name);
        lower_bound = NEGATIVE_INFINITY;
        upper_bound = INFINITY;
    }
    else if (isdigit(*ptr) || *ptr == '-') {
        temp = strtod(ptr, &ptr);

        if (strstr(ptr, "<")) {
            ptr = strstr(ptr, "<") + 2;
            lower_bound = temp;

            sscanf(ptr, "%s", var_name);
            ptr += strlen(var_name);

            while (isspace(*ptr)) ptr++;
            if (*ptr != '\0' && *ptr != '\n' && strstr(ptr, "<")) {
                ptr = strstr(ptr, "<") + 2;
                upper_bound = strtod(ptr, NULL);
            }
        }
        else {
            upper_bound = temp;
            ptr = strstr(ptr, ">") + 2;

            sscanf(ptr, "%s", var_name);
            ptr += strlen(var_name);

            while (isspace(*ptr)) ptr++;
            if (*ptr != '\0' && *ptr != '\n' && strstr(ptr, ">")) {
                ptr = strstr(ptr, ">") + 2;
                lower_bound = strtod(ptr, NULL);
            }
        }
    }
    else {
        sscanf(ptr, "%s", var_name);

        if (strstr(ptr, "<")) {
            ptr = strstr(ptr, "<") + 2;
            upper_bound = strtod(ptr, NULL);
        } else if (strstr(ptr, ">")) {
            ptr = strstr(ptr, ">") + 2;
            lower_bound = strtod(ptr, NULL);
        }
    }

    tempBounds->var_name = strdup(var_name);
    tempBounds->lower_bound = lower_bound;
    tempBounds->upper_bound = upper_bound;
    add_bound(bounds_list, tempBounds);

    /*
    printf("Variable: %s\n", var_name);
    printf("Lower Bound: %.6f\n", lower_bound);
    printf("Upper Bound: %.6f\n", upper_bound);
    */

    /* udělat stack, kam budu cpát var_name a bounds? pak jen na konci bindnu */
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