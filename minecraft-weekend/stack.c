#include "Stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

Stack* create_stack(size_t itemSize, size_t capacity) {
	Stack* stack = malloc(sizeof(Stack));
	stack->size = 0;
	stack -> capacity = capacity;
	stack->items = malloc(itemSize * capacity);
	stack->itemSize = itemSize;
	return stack;
};

void push_stack(Stack* stack, void* item) {
	if (stack->size == stack->capacity) {
		stack->capacity *= 2;
		stack->items = realloc(stack->items, stack->itemSize * stack->capacity);
	}
	void* dest = (char*)stack->items + (stack->size * stack->itemSize);
	memcpy(dest, item, stack->itemSize);

	stack->size++;
};

void pop_stack(Stack* stack, void* out) {
	void* src = (char*)stack->items + ((stack->size - 1) * stack->itemSize);
	memcpy(out, src, stack->itemSize);
	
	//stack->items = realloc(stack->items, stack->itemSize * (stack->size - 1));
	stack->size--;
};

void get_stack(Stack* stack, void* out) {
	void* src = (char*)stack->items + ((stack->size - 1) * stack->itemSize);
	memcpy(out, src, stack->itemSize);
};

void print_stack(Stack* stack) {
	for (int i = 0; i < stack->size; i++) {
		int out;
		void* src = (char*)stack->items + (i * stack->itemSize);
		memcpy(&out, src, stack->itemSize);
		printf("%d ", out);
	}
	printf("\n");
}