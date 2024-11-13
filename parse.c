#include "parse.h"
#include "ajaj/parse.h"

#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "validate.h"

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

int process_lines(char **lines) {
    int i, end_reached = 0, current_section = -1;
    char *line, *comment_start;

    SectionBuffers *buffers = create_section_buffers(INITIAL_SIZE);
    if (!buffers) {
        return 93;
    }

    for (i = 0; lines[i] != NULL; i++) {
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
            end_reached = 1;
            continue;
        }

        if (end_reached) {
            free_section_buffers(buffers);
            return 11;
        }

        switch (current_section) {
            case 1:
                add_line_to_buffer(&buffers->objective_lines, &buffers->objective_count, line);
                break;
            case 2:
                add_line_to_buffer(&buffers->subject_to_lines, &buffers->subject_to_count, line);
                break;
            case 3:
                add_line_to_buffer(&buffers->general_lines, &buffers->general_count, line);
                break;
            case 4:
                add_line_to_buffer(&buffers->bounds_lines, &buffers->bounds_count, line);
                break;
            default:
                free_section_buffers(buffers);
                return 93;
        }
    }

    parse_lines(buffers);

    free_section_buffers(buffers);
    return 0;
}

int parse_lines(SectionBuffers *buffers) {
    General_vars *general_vars;
    Bounds *bounds;
    int i;

    if(!buffers) {
        return 93;
    }

    general_vars = create_general_vars(INITIAL_SIZE);
    if(!general_vars) {
        return 93;
    }

    bounds = create_bounds(INITIAL_SIZE);
    if(!bounds) {
        free_general_vars(general_vars);
        return 93;
    }

    for (i = 0; i < buffers->general_count; i++) {
        parse_generals(general_vars, buffers->general_lines[i]);
    }

    for (i = 0; i < buffers->bounds_count; i++) {
        parse_bounds(bounds, buffers->bounds_lines[i]);
    }

    for (i = 0; i < buffers->subject_to_count; i++) {
        /*parse_subject_to(buffers->subject_to_lines[i], matrix, general_vars);*/
    }

    for (i = 0; i < buffers->objective_count; i++) {
        printf("Objectives: \n");
        if(i == 0) {
            /*matrix->type = strdup(buffers->objective_lines[i]);*/
            continue;
        }
        parse_objectives(remove_spaces(buffers->objective_lines[i]), NULL, general_vars);
    }

    for(i = 0; i < bounds->num_vars; i++) {
        printf("%s | %.6f | %.6f\n", bounds->var_names[i], bounds->lower_bound[i], bounds->upper_bound[i]);
    }

    free_general_vars(general_vars);
    free_bounds(bounds);
    return 0;
}

SectionBuffers* create_section_buffers(int initial_size) {
    SectionBuffers *buffers = (SectionBuffers *)malloc(sizeof(SectionBuffers));
    if (!buffers) {
        return NULL;
    }

    buffers->general_lines = malloc(initial_size * sizeof(char *));
    if (!buffers->general_lines) {
        free_section_buffers(buffers);
        return NULL;
    }

    buffers->subject_to_lines = malloc(initial_size * sizeof(char *));
    if (!buffers->subject_to_lines) {
        free_section_buffers(buffers);
        return NULL;
    }

    buffers->objective_lines = malloc(initial_size * sizeof(char *));
    if (!buffers->objective_lines) {
        free_section_buffers(buffers);
        return NULL;
    }

    buffers->bounds_lines = malloc(initial_size * sizeof(char *));
    if (!buffers->bounds_lines) {
        free_section_buffers(buffers);
        return NULL;
    }

    buffers->general_count = 0;
    buffers->subject_to_count = 0;
    buffers->objective_count = 0;
    buffers->bounds_count = 0;

    return buffers;
}

void free_section_buffers(SectionBuffers *buffers) {
    int i;

    if (buffers->general_lines) {
        printf("%d\n", buffers->general_count);
        for (i = 0; i < buffers->general_count; i++) {
            free(buffers->general_lines[i]);
        }
        free(buffers->general_lines);
    }

    if (buffers->subject_to_lines) {
        for (i = 0; i < buffers->subject_to_count; i++) {
            free(buffers->subject_to_lines[i]);
        }
        free(buffers->subject_to_lines);
    }

    if (buffers->objective_lines) {
        for (i = 0; i < buffers->objective_count; i++) {
            free(buffers->objective_lines[i]);
        }
        free(buffers->objective_lines);
    }

    if (buffers->bounds_lines) {
        for (i = 0; i < buffers->bounds_count; i++) {
            free(buffers->bounds_lines[i]);
        }
        free(buffers->bounds_lines);
    }

    free(buffers);
}

void add_line_to_buffer(char ***buffer, int *count, char *line) {
    char **temp;

    temp = realloc(*buffer, (*count + 1) * sizeof(char *));
    if (!temp) {
        printf("Memory reallocation failed.\n");
        return;
    }

    *buffer = temp;

    (*buffer)[*count] = strdup(line);
    if (!(*buffer)[*count]) {
        printf("Memory allocation for line failed.\n");
        return;
    }

    (*count)++;
}

double parse_coefficient(const char *token) {
    char *end_ptr;
    double coeff = strtod(token, &end_ptr);
    if (end_ptr == token) {
        return (token[0] == '-') ? -1.0 : 1.0;
    }
    return coeff;
}

int extract_variable_and_coefficient(char *segment, char *variable, double *coefficient) {
    char *ptr = segment;
    char coeff_buffer[32] = {0};
    int i = 0, index;
    char *e;

    e = strchr(ptr, '*');
    if(e) {
        index = (int)(e - ptr);
        memmove(&ptr[index], &ptr[index + 1], strlen(ptr) - index);
    }

    while (*ptr && (isdigit(*ptr) || *ptr == '.' || *ptr == '-')) {
        coeff_buffer[i++] = *ptr;
        ptr++;
    }

    if (i > 0) {
        *coefficient = parse_coefficient(coeff_buffer);
        if (*ptr == '*') {
            ptr++;
        }
    } else {
        *coefficient = 1.0;
    }

    strcpy(variable, ptr);

    printf("Segment: %s | Varible: %s | Coeff: %f\n", segment, variable, *coefficient);
    return is_valid_string(variable) ? -1 : 0;
}

int parse_objectives(char *expression, Matrix *matrix, General_vars *general_vars) {
    char variable[64];
    double coefficient;
    char *token;
    int var_index = 0;

    char modified_expression[256];
    int j = 0, i;

    for (i = 0; expression[i] != '\0'; i++) {
        if (expression[i] == '-') {
            if (i > 0 && expression[i - 1] != '+' && expression[i - 1] != '-') {
                modified_expression[j++] = '+';
            }
        }
        modified_expression[j++] = expression[i];
    }
    modified_expression[j] = '\0';

    printf("Modified Expression: %s\n", modified_expression);

    token = strtok(modified_expression, "+");
    while (token != NULL) {
        token = trim_white_space(token);
        printf("Token: %s\n", token);
        if (extract_variable_and_coefficient(token, variable, &coefficient) == 0) {
            if (!is_var_known(general_vars, variable)) {
                printf("Unknown variable '%s'!\n", variable);
                return 11;
            }
            printf("Coeff: %f\n", coefficient);
            /* matrix->objectives_row[var_index] = coefficient; */
            var_index++;
        } else {
            return -1;
        }

        token = strtok(NULL, "+");
    }

    return 0;
}


int parse_subject_to(char *line, Matrix *matrix, General_vars *general_vars) {

    return 0;
}

int parse_generals(General_vars *general_vars, char *line) {
    char *token;

    token = strtok(line, " ");
    while(token) {
        if(is_valid_string(trim_white_space(token))) {
            return 11;
        }
        add_variable(general_vars, trim_white_space(token));
        token = strtok(NULL, " ");
    }

    return 0;
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