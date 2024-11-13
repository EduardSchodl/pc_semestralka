
#include "stack.h"

#include <stdlib.h>

Stack *create_stack(const int size) {
    Stack *temp;

    temp = (Stack *)malloc(sizeof(Stack));
    if (!temp) {
        return NULL;
    }

    temp->arr = malloc(size * sizeof(double));
    if (!temp->arr) {
        free(temp);
        return NULL;
    }

    temp->capacity = size;
    temp->top = -1;

    return temp;
}
void push(Stack *stack, char value) {
    if (!stack->arr) {
        return;
    }

    if(stack->top >= stack->capacity - 1) {
        resize_stack(stack);
    }

    stack->arr[++stack->top] = value;
}

int pop(Stack *stack, double *var) {
    if (!stack) {
        return 0;
    }

    if(stack->top < 0) {
        return 0;
    }

    if(var) {
        *var = stack->arr[stack->top];
    }

    stack->top--;

    return 1;
}

void resize_stack(Stack *stack) {
    int new_capacity = stack->capacity + 10;

    double *new_arr = realloc(stack->arr, new_capacity * sizeof(double));
    if (!new_arr) {
        return;
    }

    stack->arr = new_arr;
    stack->capacity = new_capacity;
}

void free_stack(Stack *stack) {
    if (stack) {
        if (stack->arr) {
            free(stack->arr);
        }
        free(stack);
        stack = NULL;
    }
}