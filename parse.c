#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"
#include "validate.h"

int process_lines(char **lines) {
    int i = 0, end_reached = 0;
    char *line, *comment_start;
    int current_section = -1;
    int response = 1;

    General_vars *general_vars;
    Bounds *bounds;
    Constraints *constraints;
    Objectives *objectives;

    /* sanity check */
    if(!lines) {
        return 93;
    }

    /* allocate GeneralVars */
    general_vars = create_general_vars(INITIAL_SIZE);
    if(!general_vars) {
        return 93;
    }
    objectives = create_objectives(INITIAL_SIZE);
    if(!objectives) {
        free_general_vars(general_vars);
        return 93;
    }
    bounds = create_bounds(INITIAL_SIZE);
    if(!bounds) {
        free_objectives(objectives);
        free_general_vars(general_vars);
        return 93;
    }
    constraints = create_constraints(INITIAL_SIZE);
    if(!constraints) {
        free_objectives(objectives);
        free_general_vars(general_vars);
        free_bounds(bounds);
        return 93;
    }

    /* process each line */
    for (i = 0; lines[i] != NULL; i++) {
        if(response != 1) {
            free_structures(general_vars, objectives, constraints, bounds);
            return response;
        }

        line = trim_white_space(lines[i]);

        comment_start = strstr(line, "\\");
        if (comment_start != NULL) {
            *comment_start = '\0';
        }

        if (strlen(line) == 0) {
            continue;
        }

        if (strcasecmp(line, "Maximize") == 0 || strcasecmp(line, "Minimize") == 0) {
            current_section = 1;
            objectives->type = strdup(strcasecmp(line, "Maximize") == 0 ? "Maximize" : "Minimize");

            continue;
        }

        if (strcasecmp(line, "Subject To") == 0) {
            current_section = 2;
            continue;
        }

        if (strcasecmp(line, "Generals") == 0) {
            current_section = 3;
            continue;
        }

        if (strcasecmp(line, "Bounds") == 0) {
            current_section = 4;
            continue;
        }

        if (strcasecmp(line, "End") == 0) {
            /* idk zatim, něco kontrolovat */
            end_reached = 1;
            continue;
        }

        if (end_reached) {
            free_structures(general_vars, objectives, constraints, bounds);
            return 11;
        }

        switch(current_section) {
            case 1:
                printf("Objective: %s\n", line);
                /* před každým parse_... kontrolovat znak po znaku, zda obsahuje nepovolené znaky */
                parse_objectives(objectives, line);
                break;
            case 2:
                printf("Constraint: %s\n", line);
                /*parse_constraints(constraints, line);*/
                break;
            case 3:
                printf("Generals: %s\n", line);
                response = parse_generals(general_vars, line);
                break;
            case 4:
                printf("Bound: %s\n", line);
                response = parse_bounds(bounds, line);
                break;
            default:
                free_structures(general_vars, objectives, constraints, bounds);
                return 93;
        }
    }

    /* valid varname in bounds */
    for(i = 0; i < bounds->num_vars; i++) {
        if(is_invalid_string(bounds->var_names[i])) {
            free_structures(general_vars, objectives, constraints, bounds);
            return 11;
        }
    }

    /* unknown variables in bounds */
    for(i = 0; i < bounds->num_vars; i++) {
        if(!is_var_known(general_vars, bounds->var_names[i])) {
            printf("Unknown variable '%s'!\n", bounds->var_names[i]);
            free_structures(general_vars, objectives, constraints, bounds);
            return 10;
        }
    }

    for(i = 0; i < general_vars->num_general_vars; i++) {
        printf("%s | %.6f | %.6f\n", general_vars->general_vars[i], bounds->lower_bound[i], bounds->upper_bound[i]);
    }

    free_structures(general_vars, objectives, constraints, bounds);

    return 1;
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

char *remove_spaces(char *str)
{
    int count = 0, i;

    for (i = 0; str[i]; i++) {
        if (str[i] != ' ') {
            str[count++] = str[i];
        }
    }

    str[count] = '\0';

    return str;
}

void free_structures(General_vars *general_vars, Objectives *objectives, Constraints *constraints, Bounds *bounds) {
    if (general_vars) {
        free_general_vars(general_vars);
    }
    if (objectives) {
        free_objectives(objectives);
    }
    if (constraints) {
        free_constraints(constraints);
    }
    if (bounds) {
        free_bounds(bounds);
    }
}

void parse_objectives(Objectives *objectives, char *line) {

}

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

Objectives *create_objectives(int initial_size) {
    int i;
    Objectives *temp;

    temp = malloc(sizeof(Objectives));
    if (!temp) {
        return NULL;
    }

    temp->type = NULL;
    temp->num_vars = initial_size;
    temp->var_names = malloc(initial_size * sizeof(char *));
    if (!temp->var_names) {
        free(temp);
        return NULL;
    }

    for (i = 0; i < initial_size; i++) {
        temp->var_names[i] = NULL;
    }

    return temp;
}

void free_objectives(Objectives *objectives) {
    int i;

    if (!objectives) {
        return;
    }

    printf("Freeing objectives\n");

    if (objectives->var_names) {
        for (i = 0; i < objectives->num_vars; i++) {
            if (objectives->var_names[i]) {
                free(objectives->var_names[i]);
                objectives->var_names[i] = NULL;
            }
        }
        free(objectives->var_names);
        objectives->var_names = NULL;
    }

    if (objectives->type) {
        free(objectives->type);
        objectives->type = NULL;
    }

    free(objectives);
    objectives = NULL;
}



Constraints *create_constraints(const int initial_size) {
    Constraints *temp;

    temp = (Constraints *)malloc(sizeof(Constraints));
    if (!temp) {
        return NULL;
    }

    temp->relation = NULL;
    temp->rhs = 0.0;

    return temp;
}

void free_constraints(Constraints *constraints) {
    printf("Freeing constraints\n");

    if (constraints) {
        if (constraints->relation) {
            free(constraints->relation);
        }
        free(constraints);
    }
}

int parse_generals(General_vars *general_vars, char *line) {
    char *token;

    token = strtok(line, " ");
    while(token) {
        if(is_invalid_string(trim_white_space(token))) {
            return 11;
        }
        add_variable(general_vars, trim_white_space(token));
        token = strtok(NULL, " ");
    }

    return 1;
}

void add_bound(Bounds *bounds, const char *var_name, const double lower_bound, const double upper_bound) {
    int new_size;
    char **new_var_names;
    double *new_lower_bound, *new_upper_bound;

    if (!bounds) {
        return;
    }

    if (bounds->num_vars >= bounds->max_vars) {
        new_size = bounds->max_vars * 2;

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

    if(is_invalid_string(var_name)) {
        printf("invalidni varname '%s'\n", var_name);
        return 11;
    }

    /* save the bounds to the bounds list */
    add_bound(bounds, var_name, lower_bound, upper_bound);

    return 1;
}

void parse_constraints(Constraints *constraints, char *line) {

}

void add_variable(General_vars *gv, const char *var_name) {
    int new_size;
    char **new_general_vars;

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
