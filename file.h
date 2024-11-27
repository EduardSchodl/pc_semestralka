#ifndef __LIB__
#define __LIB__

#define LP_EXT ".lp"
#define OUTPUT_FILE_EXT ".txt"
#define LINE_MAX_SIZE 1024
#define MAX_PATH_LENGTH 256
#define MAX_LINES 100

#include <stdio.h>

#include "lp.h"

typedef struct {
    char **general_lines;
    int general_count;
    char **subject_to_lines;
    int subject_to_count;
    char **objective_lines;
    int objective_count;
    char **bounds_lines;
    int bounds_count;
} SectionBuffers;

int check_filename_ext(const char *filename, const char *ext);
char *get_output_file(const int argc, char **argv);
char *get_input_file(const int argc, char **argv);
int file_exists(const char *file_path);
FILE *open_output_file(char *file_path);
FILE *open_input_file(const char *file_path);
int read_store_input_file(FILE *input_file, SectionBuffers *section_buffers);

int open_file(char *file_path, char *mode, FILE **file);
void write_output_file(FILE *output_file, SimplexTableau *simplex_tableau, General_vars *general_vars);

#endif