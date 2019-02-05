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
