#include <stdio.h>
#include <string.h>
#include "validate.h"

int is_var_known(General_vars *general_vars, char *var_name){
    int i;

    for(i = 0; i < general_vars->number_of_vars; i++) {
        if(strcmp(var_name, general_vars->vars[i].name) == 0) {
            return 1;
        }
    }

    return 0;
}

int is_variable_used(const char *name, GeneralVars *general_vars, int general_var_count) {
    for (int i = 0; i < general_var_count; i++) {
        if (strcmp(general_vars[i].name, name) == 0) {
            general_vars[i].used = 1;
            return 1;
        }
    }
    return 0;
}

void check_unused_variables(GeneralVars *general_vars,  int general_var_count) {
    for (int i = 0; i < general_var_count; i++) {
        if (!general_vars[i].used) {
            printf("Warning: Unused variable '%s'.\n", general_vars[i].name);
        }
    }
    for (int i = 0; i < general_var_count; i++) {
        if (!is_variable_used(constraint_vars[i].name)) {
            printf("Error: Unknown variable '%s' used in constraints.\n", constraint_vars[i].name);
        }
    }
}