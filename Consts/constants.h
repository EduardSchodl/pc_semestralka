#ifndef __CONSTANTS__
#define __CONSTANTS__

#ifndef INFINITY
#define INFINITY (1.0 / 0.0)
#define NEGATIVE_INFINITY (-INFINITY)
#endif

#define DEFAULT_LOWER_BOUND 0.0
#define DEFAULT_UPPER_BOUND INFINITY
#define MAX_TOKENS 256
#define MAX_VAR_NAME 256
#define MAX_LINE_SIZE 1024
#define MAX_LINES 100
#define MAX_PATH_LENGTH 256
#define LP_EXT ".lp"
#define OUTPUT_FILE_EXT ".txt"
#define MAX_BUFFER_SIZE 256
#define MAX_COEFF_BUFFER 32
#define MAX_TERMS 100
#define MAX_STACK_SIZE 100
#define INITIAL_SIZE 2
#define BOUNDS_INVALID_CHARS "(){}[]/*+"
#define INVALID_STRING_CHARS "+-*^<>=()[].,:"

#endif