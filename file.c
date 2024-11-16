#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#ifdef _WIN32
    #include <io.h>
#elif __linux__
    #include <inttypes.h>
    #include <unistd.h>
    #define __int64 int64_t
    #define _close close
    #define _read read
    #define _lseek64 lseek64
    #define _O_RDONLY O_RDONLY
    #define _open open
    #define _lseeki64 lseek64
    #define _lseek lseek
    #define stricmp strcasecmp
#endif
#include "file.h"
#include "parse.h"

/* Define long options */
struct option long_options[] = {
    {"output", required_argument, 0, 'O'}, /* Corresponds to --output */
    {0, 0, 0, 0} /* End of options */
};

int check_filename_ext(const char *filename, const char *ext) {
    char *dot;

    dot = strrchr(filename, '.');
    if(!dot || strcmp(dot, ext) != 0) return 1;
    return 0;
}

char *get_output_file(const int argc, char **argv) {
    int opt;
    int option_index = 0;
    char *output_file_path = NULL;

    while ((opt = getopt_long(argc, argv, "o:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'o': /* Short option -o */
            case 'O': /* Long option --output */
                if(optarg[0] == '-') {
                    printf("Error: Invalid argument for option '-%c': %s!\n", opt, optarg);
                    return NULL;
                }
                output_file_path = optarg;
                break;
            default:
                break;
        }
    }

    printf("Output path: %s\n", output_file_path);
    return output_file_path;
}

char *get_input_file(const int argc, char **argv) {
    char *input_file = NULL;

    /* After options, remaining argument should be the input file */
    if (optind < argc) {
        input_file = malloc(MAX_PATH_LENGTH);

        strncpy(input_file, argv[optind], MAX_PATH_LENGTH - 1);
        input_file[MAX_PATH_LENGTH - 1] = '\0';

        printf("Input file: %s\n", input_file);
    } else {
        printf("Error: No input file specified.\n");
    }

    return input_file;
}

int file_exists(const char *file_path) {
    if (access(file_path, F_OK) == 0) {
        return 1;
    }

    return 0;
}

FILE *open_output_file(char *file_path) {
    FILE *file;

    if (!file_path) return NULL;

    file = fopen(file_path, "w");
    if (!file) {
        printf("Invalid output destination!\n");
    }
    return file;
}

FILE *open_input_file(const char *file_path) {
    FILE *file;

    if (!file_exists(file_path)) {
        printf("Input file not found!\n");
        return NULL;
    }

    file = fopen(file_path, "r");
    if (!file) {
        printf("Could not read input file!\n");
    }

    return file;
}

int read_store_input_file(FILE *input_file, SectionBuffers *section_buffers) {
    char line[LINE_MAX_SIZE];
    char *comment_start;
    int current_section = -1;
    int end_reached = 0;

    /* sanity check */
    if(!input_file || !section_buffers) {
        return 93;
    }

    /* read all lines from the input file */
    while(fgets(line, LINE_MAX_SIZE, input_file)) {
        trim_white_space(line);
        line[strcspn(line, "\n")] = '\0';

        comment_start = strstr(line, "\\");
        if (comment_start != NULL) {
            *comment_start = '\0';
        }

        if (strlen(line) == 0) {
            continue;
        }

        if (end_reached) {
            printf("End reached gentleman\n");
            exit(11);
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

        switch (current_section) {
            case 1:
                add_line_to_buffer(&section_buffers->objective_lines, &section_buffers->objective_count, line);
                break;
            case 2:
                add_line_to_buffer(&section_buffers->subject_to_lines, &section_buffers->subject_to_count, line);
                break;
            case 3:
                add_line_to_buffer(&section_buffers->general_lines, &section_buffers->general_count, line);
                break;
            case 4:
                add_line_to_buffer(&section_buffers->bounds_lines, &section_buffers->bounds_count, line);
                break;
            default:
                return 93;
        }
    }

    return 0;
}