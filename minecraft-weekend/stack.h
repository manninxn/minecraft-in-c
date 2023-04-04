#ifndef STACK_H
#define STACK_H
#include <stdio.h>

typedef struct {
	int size;
	size_t itemSize;
	void* items;
} Stack;

Stack* create_stack(size_t itemSize);

void push_stack(Stack* stack, void* item);

void pop_stack(Stack* stack, void* out);

void get_stack(Stack* stack, void* out);

void print_stack(Stack* stack);

#endif