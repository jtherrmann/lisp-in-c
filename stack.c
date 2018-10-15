// stack.c
// Source for the stack.


#include <assert.h>
#include <stdio.h>

#include "obj.h"
#include "print.h"
#include "stack.h"


// ============================================================================
// Private functions
// ============================================================================

// print_stack
// Print the stack.
void print_stack() {
    for (int i = 1; i <= sp; ++i) {
	print_obj(stack[i]);
	printf("  ");
    }
    printf("\n");
}


// ============================================================================
// Public functions
// ============================================================================

// push
// Push an object to the stack.
void push(LispObject * obj) {
    assert(sp < STACK_SIZE - 1);

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
    assert(sp > 0);

    --sp;

    if (stack_output) {
	printf("pop   ");
	print_stack();
	printf("\n");
    }
}
