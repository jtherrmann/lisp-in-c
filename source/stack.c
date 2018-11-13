// stack.c
// Source for the stack.


#include <stdio.h>

#include "obj.h"
#include "print.h"
#include "stack.h"


// ============================================================================
// Public functions
// ============================================================================

// push
// Push an object to the stack.
void push(LispObject * obj) {
    if (sp >= STACK_SIZE - 1) {
	printf("\nStack overflow.\n");
	exit(1);
    }

    ++sp;
    stack[sp] = obj;

    if (stack_output) {
	printf("push  ");
	print_stack();
	printf("\n");
    }
}


// pop
// Decrement the stack pointer.
void pop() {
    if (sp <= 0) {
	printf("\nStack underflow.\n");
	exit(1);
    }

    --sp;

    if (stack_output) {
	printf("pop   ");
	print_stack();
	printf("\n");
    }
}


// print_stack
// Print the stack.
void print_stack() {
    for (long i = 1; i <= sp; ++i) {
	print_obj(stack[i]);
	printf("  ");
    }
    printf("\n");
}
