// TODO:
// - address TODO/FIXME throughout files
// - temp sources list:
//   - http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
//   - https://carld.github.io/2017/06/20/lisp-in-less-than-200-lines-of-c.html
// - remove unneeded #include lines
// - document (b_ for builtin):
// Function names are prefixed with b_ if they implement Lisp functions of the
// same name (without the prefix). For example, b_cons, b_car, and b_cdr
// implement cons, car, and cdr.
// - check for malloc failure wherever it's used; look for other funcs that
//   need checked for errors
// - document (source:
//   http://www.buildyourownlisp.com/chapter4_interactive_prompt#editing_input):
//   - install debian package libedit-dev
//   - use -ledit option with gcc to link to editline
//     - record full gcc command (see ccc alias) used to compile, as well as
//       Debian version and gcc version know to work
//       - add -std=c99 option to alias?

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
    printf("\nAbort! Stack pointer is %d but should be 0.\nStack contents: ", sp);
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
		obj = b_eval(obj, LISP_EMPTY);

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
