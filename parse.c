#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "validate.h"
#include "Bounds/bounds.h"
#include "parse.h"

#include "file.h"
#include "Generals/generals.h"

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

char *remove_spaces(char *str){
    int count = 0, i;

    for (i = 0; str[i]; i++) {
        if (str[i] != ' ') {
            str[count++] = str[i];
        }
    }

    str[count] = '\0';

    return str;
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
        if(!is_var_known(general_vars, bounds->var_names[i])) {
            printf("Unknown variable name: '%s'\n", bounds->var_names[i]);
            return 11;
        }
    }

    for (i = 0; i < buffers->subject_to_count; i++) {
        parse_subject_to(remove_spaces(buffers->subject_to_lines[i]), NULL, general_vars);
    }

    for (i = 0; i < buffers->objective_count; i++) {
        if(i == 0) {
            matrix->type = strdup(buffers->objective_lines[i]);
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

    if(!buffers) {
        return;
    }

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

    (*buffer)[*count] = strdup(trim_white_space(line));
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
    parse_equation(expression, general_vars);

    return 0;
}

int parse_equation(const char *expression, General_vars *general_vars) {
    char *token;
    char variable[64];
    double coefficient;

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
        } else {
            return 1;
        }

        token = strtok(NULL, "+");
    }

    return 0;
}

int parse_subject_to(char *line, Matrix *matrix, General_vars *general_vars) {
    char *left_side;
    char *right_side;
    char *delim;
    char *delim_pos;
    char *name = NULL;
    char *name_pos;

    if ((name_pos = strstr(line, ":"))) {
        *name_pos = '\0';
        name = trim_white_space(line);
        line = name_pos + 1;
    }

    if (strstr(line, "<=")) {
        delim = "<=";
    } else if (strstr(line, ">=")) {
        delim = ">=";
    } else if (strstr(line, "<")) {
        delim = "<";
    } else if (strstr(line, ">")) {
        delim = ">";
    } else {
        return 1;
    }

    printf("Delimiter: %s\n", delim);

    delim_pos = strstr(line, delim);
    if (delim_pos == NULL) {
        printf("Error: Delimiter not found.\n");
        return 1;
    }

    *delim_pos = '\0';

    left_side = trim_white_space(line);

    right_side = trim_white_space(delim_pos + strlen(delim));

    printf("Left side: %s\n", left_side);
    printf("Right side: %s\n", right_side);

    parse_equation(left_side, general_vars);

    return 0;
}

void prepare_for_simplex(int *var_num, int *subject_to_count) {

}