#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#ifdef _WIN32
    #include <io.h>
    #define LINE_BREAK "\n"
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
    #include <strings.h>
    #define LINE_BREAK "\r"
#endif
#include "file.h"

#include <ctype.h>

#include "../Parse/parse.h"
#include "../LProblem/lp.h"
#include "../Generals/generals.h"
#include "../Validate/validate.h"

struct option long_options[] = {
    {"output", required_argument, 0, 'O'},
    {0, 0, 0, 0}
};

int check_filename_ext(const char *filename, const char *ext) {
    char *dot;

    if (!filename || !ext) {
        return 1;
    }

    dot = strrchr(filename, '.');
    if(!dot || strcmp(dot, ext) != 0) return 1;
    return 0;
}

int process_line_args(const int argc, char **argv, char *input_path, char *output_path) {
    int result_code = 0;

    if((result_code = get_output_file(argc, argv, output_path))) {
        return result_code;
    }

    if((result_code = get_input_file(argc, argv, input_path))){
        return result_code;
    }

    return result_code;
}

int get_output_file(const int argc, char **argv, char *output_path) {
    int option;
    int option_index = 0;

    while ((option = getopt_long(argc, argv, "o:", long_options, &option_index)) != -1) {
        switch (option) {
            case 'o': /* Short option -o */
            case 'O': /* Long option --output */
                if (optarg[0] == '-') {
                    printf("Error: Invalid argument for option '-%c': %s!\n", option, optarg);
                    return 93;
                }

                if(optarg) {
                    if(check_filename_ext(optarg, OUTPUT_FILE_EXT)) {
                        printf("Invalid output file extension!\n");
                    }
                    else {
                        strncpy(output_path, optarg, MAX_PATH_LENGTH - 1);
                        output_path[MAX_PATH_LENGTH - 1] = '\0';
                    }
                }
                break;
            default:
                printf("Warning: Unrecognized option '-%c'.\n", option);
                return 93;
        }
    }

    /*printf("Output path: %s\n", output_path);*/
    return 0;
}

int get_input_file(const int argc, char **argv, char *input_path) {
    /* After options, remaining argument should be the input file */
    if (optind < argc) {
        strncpy(input_path, argv[optind], MAX_PATH_LENGTH - 1);
        input_path[MAX_PATH_LENGTH - 1] = '\0';
/*
        if(check_filename_ext(input_path, LP_EXT)) {
            printf("Invalid input file extension!\n");
            return 93;
        }
*/
        /*printf("Input file: %s\n", input_path);*/
    } else {
        printf("Error: No input file specified.\n");
        return 93;
    }

    return 0;
}

int open_file(char *file_path, char *mode, FILE **file) {
    if(!file_path || !mode) {
        return 1;
    }

    *file = fopen(file_path, mode);
    if (!*file) {
        if (strcmp(mode, "r") == 0) {
            printf("Input file not found!\n");
            return 1;
        }

        if (strcmp(mode, "w") == 0) {
            printf("Invalid output destination!\n");
            return 2;
        }
    }

    return 0;
}

int load_input_file(FILE *input_file, SectionBuffers *section_buffers) {
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
        comment_start = strstr(line, "\\");
        if (comment_start != NULL) {
            *comment_start = '\0';
        }

        trim_white_space(line);
        line[strcspn(line, LINE_BREAK)] = '\0';

        if (strlen(line) == 0) {
            continue;
        }

        if (end_reached) {
            return 11;
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
                if(check_invalid_chars(line, "/^<>")) {
                    return 11;
                }
                add_line_to_buffer(&section_buffers->objective_lines, &section_buffers->objective_count, line);
                break;
            case 2:
                if(check_invalid_chars(line, "/^")) {
                    return 11;
                }
                add_line_to_buffer(&section_buffers->subject_to_lines, &section_buffers->subject_to_count, line);
                break;
            case 3:
                add_line_to_buffer(&section_buffers->general_lines, &section_buffers->general_count, line);
                break;
            case 4:
                if(bounds_valid_operators(line) || contains_invalid_operator_sequence(line)) {
                    return 11;
                }
                add_line_to_buffer(&section_buffers->bounds_lines, &section_buffers->bounds_count, line);
                break;
            default:
                return 11;
        }
    }

    if(section_buffers->objective_count <= 0 || section_buffers->subject_to_count <= 0 || section_buffers->general_count <= 0) {
        return 11;
    }

    return 0;
}

void write_output_file(FILE *output_file, double *solution, General_vars *general_vars) {
    int i;

    if(!output_file || !solution || !general_vars) {
        return;
    }

    for (i = 0; i < general_vars->num_general_vars; i++) {
        if(general_vars->used_vars[i]) {
            fprintf(output_file, "%s = %0.4f\n", general_vars->general_vars[i], solution[i]);
        }
    }
}