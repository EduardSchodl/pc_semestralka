#include <stdio.h>
#include <string.h>
#include "idklib.h"


void read_input_file(FILE *file){
    char line[256];
    int isObjective = 0;
    int isConstraints = 0;
    int isBounds = 0;
    int isGenerals = 0;

    printf("\n");

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Maximize", 8) == 0 || strncmp(line, "Minimize", 8) == 0) {
            isObjective = 1;
            isConstraints = 0;
            isBounds = 0;
            isGenerals = 0;
            continue;
        } else if (strncmp(line, "Subject To", 10) == 0) {
            isObjective = 0;
            isConstraints = 1;
            isBounds = 0;
            isGenerals = 0;
            continue;
        } else if (strncmp(line, "Generals", 8) == 0) {
            isObjective = 0;
            isConstraints = 0;
            isBounds = 0;
            isGenerals = 1;
            continue;
        } else if (strncmp(line, "Bounds", 6) == 0) {
            isObjective = 0;
            isConstraints = 0;
            isBounds = 1;
            isGenerals = 0;
            continue;
        } else if (strncmp(line, "End", 3) == 0) {
            break;
        }

        if (isObjective) {
            printf("Objective: %s", line);
        } else if (isConstraints) {
            printf("Constraint: %s", line);
        } else if (isBounds) {
            printf("Bound: %s", line);
        } else if (isGenerals) {
            printf("Generals: %s", line);
        }
    }
    printf("\n");
}

void solve_linear_programming(){

}

void print_solution(){

}