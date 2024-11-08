#ifndef __LIB__
#define __LIB__

#define LP_EXT ".lp"
#define OUTPUT_FILE_EXT ".txt"
#define LINE_MAX_SIZE 256
#define MAX_PATH_LENGTH 256
#define MAX_LINES 100

typedef struct {
    char **subject_to;
    char **maximize;
    char **generals;
    char **bounds;
    char **end;
} Sections;

int check_filename_ext(const char *filename, const char *ext);
char *get_output_file(const int argc, char **argv);
char *get_input_file(const int argc, char **argv);
int file_exists(const char *file_path);
FILE *open_output_file(char *file_path);
FILE *open_input_file(const char *file_path);
char **read_input_file(FILE *input_file);

#endif