// eval.c
// Source for eval functions.


#include <stdio.h>

#include "env.h"
#include "eval.h"
#include "misc.h"
#include "obj.h"
#include "print.h"
#include "stack.h"


// ============================================================================
// Macros
// ============================================================================

#define EVAL_ERR "Invalid expression:\n\n  "

#define INVALID_EXPR printf(EVAL_ERR); print_obj(expr); printf("\n\n");


// ============================================================================
// Private function prototypes
// ============================================================================

LispObject * get_env(LispObject * arg_names,
		     LispObject * arg_exprs,
		     LispObject * env);

int len(LispObject * list);


// ============================================================================
// Public functions
// ============================================================================

// b_eval
// Builtin Lisp function eval.
//
// Pre:
// - expr and env are protected from garbage collection.
// - env represents the local environment and is either the empty list or a
//   list of cons cells, where each cons cell's car is a symbol and its cdr is
//   the value bound to that symbol. For example, a local env in which x is
//   bound to 1 and y to 2 could be represented as ((y . 2) (x . 1)). The order
//   in which the (name . value) pairs are listed does not matter.
//
// On error:
// - Return NULL.
LispObject * b_eval(LispObject * expr, LispObject * env) {
    if (b_number_pred(expr)
	|| b_null_pred(expr)
	|| expr == LISP_T
	|| expr == LISP_F
	|| b_func_pred(expr)
	|| b_builtin_pred(expr))
	return expr;

    if (b_symbol_pred(expr)) {
	// expr is a symbol, so find the value to which it's bound.

	// Search the local env for expr's binding.
	//
	// Set binding to the first (name . value) pair in env.
	LispObject * binding = b_car(env);
	while (!b_null_pred(binding)) {

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
	LispObject * global_def = get_def(expr);
	if (global_def == NULL) {
	    INVALID_EXPR;
	    print_obj(expr);
	    printf(" is undefined\n");
	}
	return global_def;
    }

    if (!b_cons_pred(expr))
	FOUND_BUG;

    if (b_equal(b_car(expr), LISP_QUOTE)) {
	if (len(b_cdr(expr)) != 1) {
	    INVALID_EXPR;
	    print_obj(LISP_QUOTE);
	    printf(" takes 1 argument\n");
	    return NULL;
	}
    	return b_car(b_cdr(expr));
    }

    if (b_equal(b_car(expr), LISP_COND)) {

	// clauses is protected from GC because b_cdr(expr) is reachable from
	// expr, which is protected from GC as per b_eval's pre.
	LispObject * clauses = b_cdr(expr);

	LispObject * clause;
	LispObject * bool_val;
	while (!b_null_pred(clauses)) {
	    // clauses' protection from GC protects clause from GC because
	    // b_car(clauses) is reachable from clauses.
	    clause = b_car(clauses);

	    if (!b_list_pred(clause) || len(clause) != 2) {
		INVALID_EXPR;
		print_obj(LISP_COND);
		printf(" takes lists of length 2\n");
		return NULL;
	    }

	    // clause being protected from GC meets b_eval's pre that expr is
	    // protected from GC because b_car(clause) is reachable from
	    // clause; and b_eval's pre that env is protected from GC is still
	    // true here.
	    bool_val = b_eval(b_car(clause), env);

	    if (bool_val == NULL)
		return NULL;

	    if (bool_val == LISP_T)
		// clause being protected from GC meets b_eval's pre that expr
		// is protected from GC because b_car(b_cdr(clause)) is
		// reachable from clause; and b_eval's pre that env is
		// protected from GC is still true here.
		return b_eval(b_car(b_cdr(clause)), env);

	    if (bool_val != LISP_F) {
		INVALID_EXPR;
		printf("Predicate ");
		print_obj(b_car(clause));
		printf(" evaluates to non-boolean value ");
		print_obj(bool_val);
		printf("\n");
		return NULL;
	    }

	    // The new value of clauses is still protected from GC because
	    // b_cdr(clauses) is reachable from the old value of clauses.
	    clauses = b_cdr(clauses);
	}
	return LISP_NIL;
    }

    if(b_equal(b_car(expr), LISP_DEF)) {
	if (len(b_cdr(expr)) != 2) {
	    INVALID_EXPR;
	    print_obj(LISP_DEF);
	    printf(" takes 2 arguments\n");
	    return NULL;
	}

	LispObject * sym = b_car(b_cdr(expr));

	if (!b_symbol_pred(sym)) {
	    INVALID_EXPR;
	    print_obj(sym);
	    printf(" is not a symbol\n");
	    return NULL;
	}

	LispObject * def = b_eval(b_car(b_cdr(b_cdr(expr))), env);

	if (def == NULL)
	    return NULL;

	bind(sym, def);

	return def;
    }

    if(b_equal(b_car(expr), LISP_LAMBDA)) {
	if (len(b_cdr(expr)) != 2) {
	    INVALID_EXPR;
	    print_obj(LISP_LAMBDA);
	    printf(" takes 2 arguments\n");
	    return NULL;
	}

	LispObject * args_list = b_car(b_cdr(expr));
	if (!b_list_pred(args_list)) {
	    INVALID_EXPR;
	    print_obj(args_list);
	    printf(" is not a list\n");
	    return NULL;
	}

	// Check that all argument names are symbols.
	while (!b_null_pred(args_list)) {
	    if (!b_symbol_pred(b_car(args_list))) {
		INVALID_EXPR;
		print_obj(b_car(args_list));
		printf(" is not a symbol\n");
		return NULL;
	    }
	    args_list = b_cdr(args_list);
	}

	// Check for duplicate argument names.
	args_list = b_car(b_cdr(expr));
	LispObject * args_compare_list;
	while(!b_null_pred(args_list)) {
	    args_compare_list = b_cdr(args_list);
	    while (!b_null_pred(args_compare_list)) {
		if (b_equal(b_car(args_list), b_car(args_compare_list))) {
		    INVALID_EXPR;
		    printf("Duplicate argument name ");
		    print_obj(b_car(args_list));
		    printf("\n");
		    return NULL;
		}
		args_compare_list = b_cdr(args_compare_list);
	    }
	    args_list = b_cdr(args_list);
	}

	// b_eval's pre that expr is protected from GC meets get_func's pre
	// that its args are protected from GC, because b_car(b_cdr(expr)) and
	// b_car(b_cdr(b_cdr(expr))) are both reachable from expr.
	return get_func(b_car(b_cdr(expr)), b_car(b_cdr(b_cdr(expr))));
    }

    // expr represents a function application.

    LispObject * func = b_eval(b_car(expr), env);

    if (func == NULL)
	return NULL;

    LispObject * result;

    // Protect func from GC that could be triggered by calls to b_eval and/or
    // get_env, below.
    push(func);

    LispObject * arg1;
    LispObject * arg2;
    bool bool_result;
    switch(func->type) {

	// TODO: DRY up this switch block, possibly with macros

    case TYPE_BUILTIN_1_ENV:

	if (len(b_cdr(expr)) != 1) {
	    INVALID_EXPR;
	    print_obj(func);
	    printf(" takes 1 argument\n");
	    pop();  // pop func
	    return NULL;
	}

	arg1 = b_eval(b_car(b_cdr(expr)), env);

	if (arg1 == NULL) {
	    pop();  // pop func
	    return NULL;
	}

	result = func->b_func_1_env(arg1, env);

	pop();  // pop func

	if (result == NULL) {
	    INVALID_EXPR;
	    print_obj(func);
	    printf(" signaled an error\n");
	}

	return result;

    case TYPE_BUILTIN_1:

	if (len(b_cdr(expr)) != 1) {
	    INVALID_EXPR;
	    print_obj(func);
	    printf(" takes 1 argument\n");
	    pop();  // pop func
	    return NULL;
	}

	arg1 = b_eval(b_car(b_cdr(expr)), env);

	if (arg1 == NULL) {
	    pop();  // pop func
	    return NULL;
	}

	result = func->b_func_1(arg1);

	pop();  // pop func

	if (result == NULL) {
	    INVALID_EXPR;
	    print_obj(func);
	    printf(" signaled an error\n");
	}

	return result;

    case TYPE_BOOL_BUILTIN_1:

	if (len(b_cdr(expr)) != 1) {
	    INVALID_EXPR;
	    print_obj(func);
	    printf(" takes 1 argument\n");
	    pop();  // pop func
	    return NULL;
	}

	arg1 = b_eval(b_car(b_cdr(expr)), env);

	if (arg1 == NULL) {
	    pop();  // pop func
	    return NULL;
	}

	bool_result = func->b_bool_func_1(arg1);

	pop();  // pop func

	return (bool_result ? LISP_T : LISP_F);

    case TYPE_BUILTIN_2:

	if (len(b_cdr(expr)) != 2) {
	    INVALID_EXPR;
	    print_obj(func);
	    printf(" takes 2 arguments\n");
	    pop();  // pop func
	    return NULL;
	}

	arg1 = b_eval(b_car(b_cdr(expr)), env);

	if (arg1 == NULL) {
	    pop();  // pop func;
	    return NULL;
	}

	// Protect arg1 from GC that could be triggered by b_eval'ing the
	// second argument.
	push(arg1);

	arg2 = b_eval(b_car(b_cdr(b_cdr(expr))), env);

	pop(); // pop arg1

	if (arg2 == NULL) {
	    pop();  // pop func
	    return NULL;
	}

	result = func->b_func_2(arg1, arg2);

	pop();  // pop func

	return result;

    case TYPE_BOOL_BUILTIN_2:

	if (len(b_cdr(expr)) != 2) {
	    INVALID_EXPR;
	    print_obj(func);
	    printf(" takes 2 arguments\n");
	    pop();  // pop func
	    return NULL;
	}

	arg1 = b_eval(b_car(b_cdr(expr)), env);

	if (arg1 == NULL) {
	    pop();  // pop func
	    return NULL;
	}

	// Protect arg1 from GC that could be triggered by b_eval'ing the
	// second argument.
	push(arg1);

	arg2 = b_eval(b_car(b_cdr(b_cdr(expr))), env);

	pop(); // pop arg1

	if (arg2 == NULL) {
	    pop();  // pop func
	    return NULL;
	}

	bool_result = func->b_bool_func_2(arg1, arg2);

	pop();  // pop func

	return (bool_result ? LISP_T : LISP_F);

    default:
	break;
    }

    if (!b_func_pred(func)) {
	INVALID_EXPR;
	print_obj(func);
	printf(" is not a function\n");
	pop();  // pop func
	return NULL;
    }

    // func is protected from GC, so it meets get_env's pre that arg_names is
    // protected from GC, because func->args is reachable from func; and
    // b_eval's pre that expr is protected from GC meets get_env's pre that
    // arg_exprs is protected from GC, because b_cdr(expr) is reachable from
    // expr.
    LispObject * arg_names = func->args;
    LispObject * arg_exprs = b_cdr(expr);

    int len_arg_names = len(arg_names);
    if (len(arg_exprs) != len_arg_names) {
	INVALID_EXPR;
	print_obj(func);
	printf(" takes %d argument%s",
	       len_arg_names, (len_arg_names == 1 ? "\n" : "s\n"));
	pop();  // pop func
	return NULL;
    }

    // b_eval's pre that env is protected from GC meets get_env's pre that env
    // is protected from GC.
    LispObject * new_env = get_env(arg_names, arg_exprs, env);

    // Meet b_eval's pre that env is protected from GC.
    push(new_env);

    // func is protected from GC, so it meets b_eval's pre that expr is
    // protected from GC, because func->body is reachable from func.
    result = b_eval(func->body, new_env);

    pop();  // pop new_env
    pop();  // pop func

    return result;

    FOUND_BUG;
}


// ============================================================================
// Private functions
// ============================================================================

// get_env
// Get a local environment.
//
// Given a list of argument names and a list of expressions, evaluate each
// expression and bind its value to the corresponding name. Return a local
// environment of the form required by b_eval's pre.
//
// Pre:
// - arg_names, arg_exprs, and env are protected from garbage collection.
// - arg_names is the empty list or a list of symbols.
// - arg_exprs is a list of the same length as arg_names.
// - env is the current local environment, of the same form as described by
//   b_eval's pre.
//
// On error:
// - Return NULL.
LispObject * get_env(LispObject * arg_names,
		     LispObject * arg_exprs,
		     LispObject * env)
{
    LispObject * new_env = LISP_NIL;
    LispObject * binding;
    LispObject * arg_val;

    while (!b_null_pred(arg_names)) {

	// Protect new_env from GC that could be triggered by b_eval or b_cons.
	push(new_env);

	// get_env's pre that arg_exprs is protected from GC meets b_eval's pre
	// that its first arg is protected from GC, because b_car(arg_exprs) is
	// reachable from arg_exprs; and get_env's pre that env is protected
	// from GC meets b_eval's pre that its second arg is protected from GC.
	arg_val = b_eval(b_car(arg_exprs), env);

	if (arg_val == NULL) {
	    pop();  // pop new_env
	    return NULL;
	}

	// Construct a (name . value) pair.
	binding = b_cons(b_car(arg_names), arg_val);

	pop();  // pop new_env

	new_env = b_cons(binding, new_env);

	arg_names = b_cdr(arg_names);
	arg_exprs = b_cdr(arg_exprs);
    }

    return new_env;
}


// len
// Return the length of a Lisp list.
//
// Pre:
// - b_list_pred(list)
// - The last element of list is the empty list.
int len(LispObject * list) {
    int count = 0;
    while (!b_null_pred(list)) {
	++count;
	list = b_cdr(list);
    }
    return count;
}
