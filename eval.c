// eval.c
// Source for eval functions.


#include <assert.h>

#include "env.h"
#include "eval.h"
#include "obj.h"


// eval
// Evaluate an expression.
LispObject * eval(LispObject * expr) {
    if (b_numberp(expr) || b_null(expr))
	return expr;

    if (b_symbolp(expr))
	return get_def(expr);

    assert(b_consp(expr));

    if (b_equal(b_car(expr), LISP_QUOTE)) {
	// TODO: proper error
	assert(!b_equal(b_cdr(expr), LISP_NIL));
    	return b_car(b_cdr(expr));
    }

    if(b_equal(b_car(expr), LISP_DEF)) {
	// TODO: proper errors

	// Check that there are two operands.
	assert(!b_equal(b_cdr(b_cdr(expr)), LISP_NIL));
	assert(b_equal(b_cdr(b_cdr(b_cdr(expr))), LISP_NIL));

	LispObject * sym = b_car(b_cdr(expr));
	assert(b_symbolp(sym));

	LispObject * def = eval(b_car(b_cdr(b_cdr(expr))));
	bind(sym, def);

	return def;
    }

    // TODO: special forms, eval-apply, etc.
    return expr;
}
