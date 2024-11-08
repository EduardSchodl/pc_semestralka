#ifndef __VALIDATE__
#define __VALIDATE__

#define MAX_LINE_LENGTH 256
#define MAX_VARIABLES 100
#define MAX_SECTIONS 6
#define MAX_VAR_NAME 50

int is_variable_used(const char *name);
void check_unused_variables();
#endif
