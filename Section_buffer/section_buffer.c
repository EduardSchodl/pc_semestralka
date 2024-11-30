#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "section_buffer.h"
#include "../Parse/parse.h"

SectionBuffers* create_section_buffers(int initial_size) {
    SectionBuffers *buffers = NULL;

    if (!initial_size) {
        return NULL;
    }

    buffers = (SectionBuffers *)malloc(sizeof(SectionBuffers));
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

    if (!buffers) {
        return;
    }

    if (buffers->general_lines) {
        for (i = 0; i < buffers->general_count; i++) {
            /* free(&buffers->general_lines[i]); */
            free(buffers->general_lines[i]);
        }
        free(buffers->general_lines);
        buffers->general_lines = NULL;
    }

    if (buffers->subject_to_lines) {
        for (i = 0; i < buffers->subject_to_count; i++) {
            free(buffers->subject_to_lines[i]);
        }
        free(buffers->subject_to_lines);
        buffers->subject_to_lines = NULL;
    }

    if (buffers->objective_lines) {
        for (i = 0; i < buffers->objective_count; i++) {
            free(buffers->objective_lines[i]);
        }
        free(buffers->objective_lines);
        buffers->objective_lines = NULL;
    }

    if (buffers->bounds_lines) {
        for (i = 0; i < buffers->bounds_count; i++) {
            free(buffers->bounds_lines[i]);
        }
        free(buffers->bounds_lines);
        buffers->bounds_lines = NULL;
    }

    free(buffers);
    buffers = NULL;
}


void add_line_to_buffer(char ***buffer, int *count, char *line) {
    char **temp;

    if (!buffer || !*buffer || !line) {
        return;
    }

    temp = realloc(*buffer, (*count + 1) * sizeof(char *));
    if (!temp) {
        printf("Memory reallocation failed.\n");
        return;
    }

    *buffer = temp;
    trim_white_space(line);

    (*buffer)[*count] = malloc(strlen(line) + 1);
    if (!(*buffer)[*count]) {
        printf("Memory allocation for line failed.\n");
        return;
    }

    strncpy((*buffer)[*count], line, strlen(line) + 1);

    (*count)++;
}