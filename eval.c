// eval.c
// Source for eval functions.


#include <assert.h>
#include <stdio.h>

#include "env.h"
#include "eval.h"
#include "obj.h"


// eval
// Evaluate an expression.
//
// Pre: expr is protected from garbage collection.
LispObject * eval(LispObject * expr) {
    if (b_numberp(expr) || b_null(expr))
	return expr;

    if (b_symbolp(expr))
	return get_def(expr);

    assert(b_consp(expr));

    if (b_equal(b_car(expr), LISP_QUOTE)) {
	// TODO: proper error
	assert(!b_null(b_cdr(expr)));
    	return b_car(b_cdr(expr));
    }

    if(b_equal(b_car(expr), LISP_DEF)) {
	// TODO: proper errors

	// Check that there are two operands.
	assert(!b_null(b_cdr(b_cdr(expr))));
	assert(b_null(b_cdr(b_cdr(b_cdr(expr)))));

	LispObject * sym = b_car(b_cdr(expr));
	assert(b_symbolp(sym));

	bind(sym, eval(b_car(b_cdr(b_cdr(expr)))));

	// We could just return the second argument, but this acts a built-in
	// check that sym was bound successfully.
	return(eval(sym));
    }

    if(b_equal(b_car(expr), LISP_LAMBDA)) {
	// TODO: proper errors

	// Check that there are two operands.
	assert(!b_null(b_cdr(b_cdr(expr))));
	assert(b_null(b_cdr(b_cdr(b_cdr(expr)))));

	// eval's pre meets get_func's pre.
	return get_func(b_car(b_cdr(expr)), b_car(b_cdr(b_cdr(expr))));
    }

    // expr represents a function application.

    LispObject * func = eval(b_car(expr));

    switch(func->type) {

    case TYPE_BUILTIN_2:
	// TODO: check number of args
	return func->c_func(eval(b_car(b_cdr(expr))),
			    eval(b_car(b_cdr(b_cdr(expr)))));
	break;

    default:
	break;
    }

    // TODO: proper error
    assert(false);
}
