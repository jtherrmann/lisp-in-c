#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "commands.h"
#include "env.h"
#include "eval.h"
#include "gc.h"
#include "error.h"
#include "obj.h"
#include "parse.h"
#include "print.h"
#include "stack.h"


// bad_stack
// Handle an invalid stack state.
void bad_stack() {
    printf("\nAbort! Stack pointer is %ld but should be 0.\nStack contents: ", sp);
    print_stack();
    printf("\n");
    FOUND_BUG;
}


// main
// Set up the interpreter and run the REPL.
int main() {
    // Initialize the stack pointer.
    sp = 0;

    gc_output = false;
    stack_output = false;

    weakrefs_head = NULL;
    weakrefs_count = 0;

    make_initial_objs();

    // Stores the return values of parse and b_eval.
    LispObject * obj;

    printf("Welcome to Lisp!\n");
    printf("Exit with Ctrl-c\n\n");

    // REPL.
    while (true) {
	input = readline("> ");
	add_history(input);

	input_index = 0;
	skipspace();  // Meet parse's pre.

	if (input[input_index] == ':')
	    exec_command(input[input_index + 1]);
	else if (input[input_index] != INPUT_END && input[input_index] != ';') {
	    obj = parse();

	    if (sp != 0)
		bad_stack();

	    if (obj != NULL && input[input_index] != INPUT_END && input[input_index] != ';') {
		show_input_char();
		printf("%sexpected end of input but got '%c'\n",
		       PARSE_ERR, input[input_index]);
		obj = NULL;
	    }

	    if (obj != NULL) {
		// Meet b_eval's pre by protecting its first arg from GC.
		push(obj);

		// LISP_EMPTY is part of the initial set of objects protected
		// from GC, so it meets b_eval's pre that its second arg is
		// protected from GC.
		obj = b_eval(obj, LISP_EMPTY, true);

		pop();

		if (sp != 0)
		    bad_stack();

		if (obj != NULL) {
		    print_obj(obj);
		    printf("\n");
		}
	    }
	}

	free(input);
    }

    return 0;
}
