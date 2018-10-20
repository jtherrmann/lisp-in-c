// eval.c
// Source for eval functions.


#include <assert.h>
#include <stdio.h>

#include "env.h"
#include "eval.h"
#include "obj.h"
#include "stack.h"


// ============================================================================
// Private function prototypes
// ============================================================================

LispObject * get_env(LispObject * arg_names,
		     LispObject * arg_exprs,
		     LispObject * env);


// ============================================================================
// Public functions
// ============================================================================

// eval
// Evaluate an expression.
//
// Pre:
// - expr and env are protected from garbage collection.
// - env represents the local environment and is either the empty list or a
//   list of cons cells, where each cons cell's car is a symbol and its cdr is
//   the value bound to that symbol. For example, a local env in which x is
//   bound to 1 and y to 2 could be represented as ((y . 2) (x . 1)). The order
//   in which the (name . value) pairs are listed does not matter.
LispObject * eval(LispObject * expr, LispObject * env) {
    if (b_numberp(expr) || b_null(expr))
	return expr;

    if (b_symbolp(expr)) {
	// expr is a symbol, so find the value to which it's bound.

	// Search the local env for expr's binding.
	//
	// Set binding to the first (name . value) pair in env.
	LispObject * binding = b_car(env);
	while (!b_equal(binding, LISP_NIL)) {

	    // If expr equals the name in the (name . value) pair, return the
	    // value.
	    if (b_equal(b_car(binding), expr))
		return b_cdr(binding);

	    // Otherwise, set binding to the next (name . value) pair in env.
	    env = b_cdr(env);
	    binding = b_car(env);
	}

	// The local env doesn't contain a binding for expr, so look it up in
	// the global env.
	return get_def(expr);
    }

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

	LispObject * def = eval(b_car(b_cdr(b_cdr(expr))), env);
	bind(sym, def);

	return def;
    }

    if(b_equal(b_car(expr), LISP_LAMBDA)) {
	// TODO: proper errors
	// TODO: ensure no duplicate arg symbols
	// TODO: ensure args are all symbols (probably check in get_func);
	// get_env's pre

	// Check that there are two operands.
	assert(!b_null(b_cdr(b_cdr(expr))));
	assert(b_null(b_cdr(b_cdr(b_cdr(expr)))));

	// eval's pre that expr is protected from GC meets get_func's pre that
	// its args are protected from GC, because b_car(b_cdr(expr)) and
	// b_car(b_cdr(b_cdr(expr))) are both reachable from expr.
	return get_func(b_car(b_cdr(expr)), b_car(b_cdr(b_cdr(expr))));
    }

    // expr represents a function application.

    LispObject * func = eval(b_car(expr), env);
    LispObject * result;

    // Protect func from GC that could be triggered by calls to eval and/or
    // get_env, below.
    push(func);

    switch(func->type) {

    case TYPE_BUILTIN_2:

	// Use an empty statement after our label to work around a quirk where
	// declarations cannot follow labels. See:
	// https://stackoverflow.com/a/18496437
	;

	// TODO: check number of args

	LispObject * arg1 = eval(b_car(b_cdr(expr)), env);

	// Protect arg1 from GC that could be triggered by eval'ing the second
	// argument.
	push(arg1);

	LispObject * arg2 = eval(b_car(b_cdr(b_cdr(expr))), env);

	pop(); // pop arg1

	result = func->c_func(arg1, arg2);

	pop();  // pop func

	return result;

    default:
	break;
    }

    assert(b_funcp(func));

    // TODO: check args names and args exprs same length (get_env's pre)

    // func is protected from GC, so it meets get_env's pre that its first arg
    // is protected from GC, because func->args is reachable from func; eval's
    // pre that expr is protected from GC meets get_env's pre that its second
    // arg is protected from GC, because b_cdr(expr) is reachable from expr;
    // and eval's pre that env is protected from GC meets get_env's pre that
    // its third arg is protected from GC.
    LispObject * new_env = get_env(func->args, b_cdr(expr), env);

    // Meet eval's pre that env is protected from GC.
    push(new_env);

    // func is protected from GC, so it meets eval's pre that expr is protected
    // from GC, because func->body is reachable from func.
    result = eval(func->body, new_env);

    pop();  // pop new_env
    pop();  // pop func

    return result;

    // TODO: proper error
    assert(false);
}


// ============================================================================
// Private functions
// ============================================================================

// get_env
// Get a local environment.
//
// Given a list of argument names and a list of expressions, evaluate each
// expression and bind its value to the corresponding name. Return a local
// environment of the form required by eval's pre.
//
// Pre:
// - arg_names, arg_exprs, and env are protected from garbage collection.
// - arg_names is the empty list or a list of symbols.
// - arg_exprs is a list of the same length as arg_names.
// - env is the current local environment, of the same form as described by
//   eval's pre.
LispObject * get_env(LispObject * arg_names,
		     LispObject * arg_exprs,
		     LispObject * env)
{
    LispObject * new_env = LISP_NIL;
    LispObject * binding;
    LispObject * arg_val;

    while (!b_equal(arg_names, LISP_NIL)) {

	// Protect new_env from GC that could be triggered by eval or b_cons.
	push(new_env);

	// get_env's pre that arg_exprs is protected from GC meets eval's pre
	// that its first arg is protected from GC, because b_car(arg_exprs) is
	// reachable from arg_exprs; and get_env's pre that env is protected
	// from GC meets eval's pre that its second arg is protected from GC.
	arg_val = eval(b_car(arg_exprs), env);

	// Construct a (name . value) pair.
	binding = b_cons(b_car(arg_names), arg_val);

	pop();

	new_env = b_cons(binding, new_env);

	arg_names = b_cdr(arg_names);
	arg_exprs = b_cdr(arg_exprs);
    }

    return new_env;
}
