// commands.c
// Source for special interpreter commands.


#include <stdio.h>

#include "commands.h"
#include "env.h"
#include "gc.h"
#include "print.h"
#include "stack.h"


// ============================================================================
// Private function prototypes
// ============================================================================

void print_weakrefs();

void print_env(bool print_hash);


// ============================================================================
// Public functions
// ============================================================================

// exec_command
// Execute a special interpreter command.
void exec_command(char cmd) {
    switch (cmd) {

    case 'w':
	print_weakrefs();
	break;

    case 'e':
	print_env(false);
	break;

    case 'E':
	print_env(true);
	break;

    case 'g':
	gc_output = !gc_output;
	printf("GC output: %s\n", gc_output ? "on" : "off");
	break;

    case 's':
	stack_output = !stack_output;
	printf("stack output: %s\n", stack_output ? "on" : "off");
	break;

    default:
	printf("Unrecognized command.\n");
	break;
    }
}


// ============================================================================
// Private functions
// ============================================================================

// print_weakrefs
// Print the weak refs list.
void print_weakrefs() {
    LispObject * current = weakrefs_head;
    while (current != NULL) {
	print_obj(current);
	printf(" -> ");
	current = current->weakref;
    }
    printf("NULL\n\n");
    printf("weakrefs count: %lu\n", weakrefs_count);
}


// print_env
// Print the global environment.
void print_env(bool print_hash) {
    struct binding * b;
    for (int i = 0; i < HASHSIZE; ++i) {
	if (print_hash && env[i] != NULL) {
	    printf("---\n");
	    printf("%d\n", i);
	    printf("---\n");
	}
	for (b = env[i]; b != NULL; b = b->next) {
	    print_obj(b->name);
	    printf("\n");
	    print_obj(b->def);
	    printf("\n");
	    printf("\n");
	}
    }
}
