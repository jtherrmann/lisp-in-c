// TODO:
// - address TODO/FIXME throughout files
// - split into multiple files, *.c or *.h as appropriate
// - temp sources list:
//   - http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
//   - https://carld.github.io/2017/06/20/lisp-in-less-than-200-lines-of-c.html
// - add tests
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

#include "obj.h"
#include "gc.h"
#include "parse.h"
#include "print.h"
#include "tests.h"


// ============================================================================
// Main
// ============================================================================

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

    LISP_NIL = get_nil();
    weakrefs_head = NULL;

    // TODO: add to tests
    assert(b_car(LISP_NIL) == LISP_NIL && b_cdr(LISP_NIL) == LISP_NIL);

    run_tests();

    printf("Welcome to Lisp!\n");
    printf("Exit with Ctrl-c\n\n");

    int i = 0;  // TODO: remove when no longer needed
    while (true) {
	if (i == 10) {
	    print_weakrefs();
	    i = 0;
	}
	++i;

    	fputs("> ", stdout);
    	fgets(input, INPUT_LEN, stdin);

	input_index = 0;
	skipspace();  // Meet parse's pre.

	if (input[input_index] != INPUT_END) {
	    LispObject * obj = parse();

	    if (input[input_index] != INPUT_END) {
		printf("PARSE ERROR: multiple expressions\n");
		exit(1);
	    }

	    print_obj(obj);
	    printf("\n");
	}
    }

    return 0;
}
