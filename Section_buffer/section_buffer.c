#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "section_buffer.h"
#include "../Parse/parse.h"
#include "../Memory_manager/memory_manager.h"

SectionBuffers* create_section_buffers(int initial_size) {
    SectionBuffers *buffers = NULL;

    /* sanity check */
    if (!initial_size) {
        return NULL;
    }

    /* alokace jednotlivých částí struktury SectionBuffers */
    buffers = (SectionBuffers *)tracked_malloc(sizeof(SectionBuffers));
    if (!buffers) {
        return NULL;
    }

    buffers->general_lines = tracked_malloc(initial_size * sizeof(char *));
    if (!buffers->general_lines) {
        free_section_buffers(buffers);
        return NULL;
    }

    buffers->subject_to_lines = tracked_malloc(initial_size * sizeof(char *));
    if (!buffers->subject_to_lines) {
        free_section_buffers(buffers);
        return NULL;
    }

    buffers->objective_lines = tracked_malloc(initial_size * sizeof(char *));
    if (!buffers->objective_lines) {
        free_section_buffers(buffers);
        return NULL;
    }

    buffers->bounds_lines = tracked_malloc(initial_size * sizeof(char *));
    if (!buffers->bounds_lines) {
        free_section_buffers(buffers);
        return NULL;
    }

    /* nastavení počátečních hodnot */
    buffers->general_count = 0;
    buffers->subject_to_count = 0;
    buffers->objective_count = 0;
    buffers->bounds_count = 0;

    return buffers;
}

void free_section_buffers(SectionBuffers *buffers) {
    int i;

    /* sanity check */
    if (!buffers) {
        return;
    }

    /* uvolnění jednotlivých částí struktury SectionBuffers */
    if (buffers->general_lines) {
        for (i = 0; i < buffers->general_count; i++) {
            tracked_free(buffers->general_lines[i]);
        }
        tracked_free(buffers->general_lines);
        buffers->general_lines = NULL;
    }

    if (buffers->subject_to_lines) {
        for (i = 0; i < buffers->subject_to_count; i++) {
            tracked_free(buffers->subject_to_lines[i]);
        }
        tracked_free(buffers->subject_to_lines);
        buffers->subject_to_lines = NULL;
    }

    if (buffers->objective_lines) {
        for (i = 0; i < buffers->objective_count; i++) {
            tracked_free(buffers->objective_lines[i]);
        }
        tracked_free(buffers->objective_lines);
        buffers->objective_lines = NULL;
    }

    if (buffers->bounds_lines) {
        for (i = 0; i < buffers->bounds_count; i++) {
            tracked_free(buffers->bounds_lines[i]);
        }
        tracked_free(buffers->bounds_lines);
        buffers->bounds_lines = NULL;
    }

    tracked_free(buffers);
    buffers = NULL;
}


void add_line_to_buffer(char ***buffer, int *count, char *line) {
    char **temp;

    /* sanity check */
    if (!buffer || !*buffer || !line) {
        return;
    }

    /* realokace bufferu pro vložení nového řetězce */
    temp = tracked_realloc(*buffer, (*count + 1) * sizeof(char *));
    if (!temp) {
        printf("Memory reallocation failed.\n");
        return;
    }

    *buffer = temp;
    trim_white_space(line);

    /* alokace paměti pro řetězec */
    (*buffer)[*count] = tracked_malloc(strlen(line) + 1);
    if (!(*buffer)[*count]) {
        printf("Memory allocation for line failed.\n");
        return;
    }

    /* uložení řetězce do bufferu */
    strncpy((*buffer)[*count], line, strlen(line) + 1);

    (*count)++;
}