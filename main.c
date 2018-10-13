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

#include "env.h"
#include "eval.h"
#include "gc.h"
#include "obj.h"
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

    make_initial_objs();

    // TODO: comment explaining why set these after make_initial_objs()
    weakrefs_head = NULL;
    weakrefs_count = 0;

    assert(b_car(LISP_NIL) == LISP_NIL && b_cdr(LISP_NIL) == LISP_NIL);

    // TODO: temp
    /* run_tests(); */

    printf("Welcome to Lisp!\n");
    printf("Exit with Ctrl-c\n\n");

    int i = 0;  // TODO: remove when no longer needed
    while (true) {

	// TODO: I think a weakness of only collecting garbage here is that
	// objects generated during parsing/evaling (particularly during
	// evaling) could make the count go over the max, so the solution would
	// be to always do this check at the beginning of get_obj, but the
	// problem there is that then you need a way to add the local scope to
	// the root set, so you can mark the objs that are in use by the
	// current function but are not part of env; so you could add them to a
	// stack, but that seems wasteful because the stack would only be used
	// for marking objs and not for actually passing params or storing
	// local objs because the C call stack already does that, so then you
	// could never pass params or store local vars (where params and local
	// vars are Lisp objects) on the C call stack and instead do everything
	// manually on the virtual stack just for the sake of consistency, but
	// that seems silly...then you could also just not even do mark and
	// sweep and instead do reference counting, but then you have to handle
	// circular refs somehow...and the problem here is that none of this is
	// really being driven by an actual problem to solve because I'm just
	// trying to do it the "right" way, for the sake of learning what the
	// right way is, but there is no right way except the one that solves
	// an actual problem, which I don't have....So for now just GC here and
	// come up with a different solution if one is ever needed.
	//
	// TODO: determine good number for demo (could demo a low number, then
	// a high number to actually show memory usage rise, then fall after
	// GC)
	if (weakrefs_count > 20)
	    collect_garbage();

	if (i == 5) {
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

	    print_obj(eval(obj));
	    printf("\n");
	}
    }

    return 0;
}
