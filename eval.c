// eval.c
// Source for eval functions.


#include <assert.h>

#include "eval.h"
#include "obj.h"
#include "tests.h"  // TODO: objs_equal should be a builtin from obj.h


LispObject * eval(LispObject * expr) {
    if (b_numberp(expr) || b_null(expr))
	return expr;

    if (b_symbolp(expr))
	// TODO: eval sym by env
	return expr;

    assert(b_consp(expr));

    if (objs_equal(b_car(expr), LISP_QUOTE)) {
	// TODO: proper error
	assert(!objs_equal(b_cdr(expr), LISP_NIL));
    	return b_car(b_cdr(expr));
    }

    // TODO: special forms, eval-apply, etc.
    return expr;
}
