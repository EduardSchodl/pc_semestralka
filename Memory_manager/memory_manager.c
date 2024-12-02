#include <stdio.h>
#include <stdlib.h>
#include "memory_manager.h"

static int total_allocated_pointers = 0;

void *tracked_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr) {
        total_allocated_pointers++;
    }
    return ptr;
}

void *tracked_realloc(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (new_ptr) {
        if (!ptr) {
            total_allocated_pointers++;
        }
    }
    return new_ptr;
}

void *tracked_calloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    if (ptr) {
        total_allocated_pointers++;
    }
    return ptr;
}

void tracked_free(void *ptr) {
    if (ptr) {
        total_allocated_pointers--;
        free(ptr);
    }
}

void report_memory_usage() {
    printf("Memory Usage Report:\n");
    printf("Remaining allocations: %d\n", total_allocated_pointers);
    if (total_allocated_pointers != 0) {
        printf("Warning: Memory leak detected! Not all allocations were freed.\n");
    } else {
        printf("All allocations were successfully freed.\n");
    }
}