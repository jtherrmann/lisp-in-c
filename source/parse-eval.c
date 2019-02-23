#include <stdio.h>

#include "parse-eval.h"
#include "parse.h"
#include "error.h"
#include "eval.h"
#include "stack.h"


// ============================================================================
// Private functions
// ============================================================================

void bad_stack() {
    printf("\nAbort! Stack pointer is %ld but should be 0.\nStack contents: ",
	   stack_ptr);
    print_stack();
    printf("\n");
    FOUND_BUG;
}


// ============================================================================
// Public functions
// ============================================================================

LispObject * parse_eval(char * input_str) {
    input = input_str;

    // Stores the return values of parse and eval.
    LispObject * obj = NULL;

    input_index = 0;
    skipspace();  // Meet parse's pre.

    if (input[input_index] != INPUT_END && input[input_index] != ';') {
	obj = parse();

	if (stack_ptr != 0)
	    bad_stack();

	if (obj != NULL
	    && input[input_index] != INPUT_END
	    && input[input_index] != ';') {

	    show_input_char();
	    printf("%sexpected end of input but got '%c'\n",
		   PARSE_ERR, input[input_index]);
	    obj = NULL;
	}

	if (obj != NULL) {
	    // Meet eval's pre by protecting its first arg from GC.
	    push(obj);

	    // LISP_EMPTY is part of the initial set of objects protected
	    // from GC, so it meets eval's pre that its second arg is
	    // protected from GC.
	    obj = eval(obj, LISP_EMPTY);

	    pop();

	    if (stack_ptr != 0)
		bad_stack();
	}
    }
    return obj;
}
