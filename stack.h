#ifndef __STACK__
#define __STACK_H__

typedef struct {
    int top;
    int capacity;
    double *arr;
} Stack;

Stack *create_stack(int size);
void push(Stack *stack, char val);
int pop(Stack *stack, double *var);
void resize_stack(Stack *stack);

#endif
