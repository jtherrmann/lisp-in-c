// commands.c
// Source for special interpreter commands.


#include <stdio.h>

#include "commands.h"
#include "gc.h"
#include "print.h"


// ============================================================================
// Private function prototypes
// ============================================================================

void print_weakrefs();


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
