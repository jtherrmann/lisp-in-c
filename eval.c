// eval.c
// Source for eval functions.


#include <assert.h>

#include "eval.h"
#include "obj.h"


LispObject * eval(LispObject * expr) {
    if (b_numberp(expr) || b_null(expr))
	return expr;

    if (b_symbolp(expr))
	// TODO: eval sym by env
	return expr;

    assert(b_consp(expr));

    if (b_equal(b_car(expr), LISP_QUOTE)) {
	// TODO: proper error
	assert(!b_equal(b_cdr(expr), LISP_NIL));
    	return b_car(b_cdr(expr));
    }

    // TODO: special forms, eval-apply, etc.
    return expr;
}
