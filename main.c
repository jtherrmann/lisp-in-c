// TODO:
// - address TODO/FIXME throughout files
// - split into multiple files, *.c or *.h as appropriate
// - temp sources list:
//   - http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
//   - https://carld.github.io/2017/06/20/lisp-in-less-than-200-lines-of-c.html
// - remove unneeded #include lines
// - comment all func definitions (name, summary, pre & post); don't comment
//   prototypes (change where this has already been done)
// - document (b_ for builtin):
// Function names are prefixed with b_ if they implement Lisp functions of the
// same name (without the prefix). For example, b_cons, b_car, and b_cdr
// implement cons, car, and cdr.
// - check for malloc failure wherever it's used; look for other funcs that
//   need checked for errors

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "env.h"
#include "eval.h"
#include "gc.h"
#include "obj.h"
#include "parse.h"
#include "print.h"
#include "stack.h"


// TODO: use the library used in build your own lisp to allow arrow keys and
// history; also allow Ctrl-h as backspace
int main() {

    // TODO: remove when no longer needed
    /* long i = 1; */
    /* while (true) { */
    /* 	get_int(0); */
    /* 	if (i % 100000000 == 0) { */
    /* 	    free_all(); // without this line we get memory leak */
    /* 	    i = 1; */
    /* 	} */
    /* 	++i; */
    /* } */

    // Initialize the stack pointer.
    sp = 0;

    gc_output = false;
    stack_output = false;

    weakrefs_head = NULL;
    weakrefs_count = 0;

    make_initial_objs();
    assert(b_car(LISP_NIL) == LISP_NIL && b_cdr(LISP_NIL) == LISP_NIL);

    // Stores the return values of parse and eval.
    LispObject * obj;

    printf("Welcome to Lisp!\n");
    printf("Exit with Ctrl-c\n\n");

    while (true) {
    	fputs("> ", stdout);
    	fgets(input, INPUT_LEN, stdin);

	input_index = 0;
	skipspace();  // Meet parse's pre.

	if (input[input_index] == ':')
	    exec_command(input[input_index + 1]);
	else if (input[input_index] != INPUT_END) {
	    obj = parse();

	    // TODO: proper error
	    assert(sp == 0);

	    if (obj != NULL && input[input_index] != INPUT_END) {
		show_input_char();
		printf("%sexpected end of input but got '%c'\n",
		       PARSE_ERR, input[input_index]);
		obj = NULL;
	    }

	    if (obj != NULL) {
		// Meet eval's pre by protecting its first arg from GC.
		push(obj);

		// LISP_NIL is part of the initial set of objects protected from
		// GC, so it meets eval's pre that its second arg is protected from
		// GC.
		obj = eval(obj, LISP_NIL);

		pop();

		// TODO: proper error
		assert(sp == 0);

		if (obj != NULL) {
		    print_obj(obj);
		    printf("\n");
		}
	    }
	}
    }

    return 0;
}
