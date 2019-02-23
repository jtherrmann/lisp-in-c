// stack.c
// Source for the stack.


#include <stdio.h>

#include "env.h"
#include "error.h"
#include "obj.h"
#include "print.h"
#include "stack.h"


// ============================================================================
// Private functions
// ============================================================================

bool stack_output() {
    return get_config_bool(LISP_STACK_OUTPUT);
}


// ============================================================================
// Public functions
// ============================================================================

// push
// Push an object to the stack.
void push(LispObject * obj) {
    if (stack_ptr >= STACK_SIZE - 1) {
	printf("\nStack overflow.\n");
	exit(1);
    }

    ++stack_ptr;
    stack[stack_ptr] = obj;

    if (stack_output()) {
	printf("push  ");
	print_stack();
	printf("\n");
    }
}


// pop
// Decrement the stack pointer.
void pop() {
    if (stack_ptr <= 0) {
	printf("\nStack underflow.\n");
	exit(1);
    }

    --stack_ptr;

    if (stack_output()) {
	printf("pop   ");
	print_stack();
	printf("\n");
    }
}


// print_stack
// Print the stack.
void print_stack() {
    for (long i = 1; i <= stack_ptr; ++i) {
	print_obj(stack[i]);
	printf("  ");
    }
    printf("\n");
}
