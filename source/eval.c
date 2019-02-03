// eval.c
// Source for eval functions.


#include <stdio.h>

#include "env.h"
#include "builtins.h"
#include "eval.h"
#include "error.h"
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

LispObject * get_new_env(LispObject * arg_names,
			 LispObject * arg_exprs,
			 LispObject * env_list);


// ============================================================================
// Public functions
// ============================================================================

// b_eval
// Builtin Lisp function eval.
//
// Pre:
// - expr and env_list are protected from garbage collection.
// - env_list is the empty list or a list of local environments, where each
//   local env is either the empty list or a list of pairs, where each pair's
//   car is a symbol and its cdr is the value bound to that symbol. For
//   example, a local env in which x is bound to 1 and y to 2 could be
//   represented as ((y . 2) (x . 1)); the order in which the (name . value)
//   pairs are listed in a local env does not matter. However, names are
//   lexically scoped, so when looking up a symbol, local envs will be searched
//   in the order in which they appear in env_list; the first env listed is the
//   innermost env and the last env listed is the outermost env.
//
// On error:
// - Return NULL.
LispObject * b_eval(LispObject * expr, LispObject * env_list, bool toplevel) {
    if (b_int_pred(expr)
	|| b_null_pred(expr)
	|| expr == LISP_T
	|| expr == LISP_F
	|| expr->type == TYPE_LAMBDA
	|| is_builtin(expr))
	return expr;

    if (b_symbol_pred(expr)) {
	// expr is a symbol, so find the value to which it's bound.

	// Search env_list for expr's binding.
	LispObject * env;
	while (!b_null_pred(env_list)) {
	    env = car(env_list);

	    // Search env for expr's binding.
	    LispObject * binding;
	    while (!b_null_pred(env)) {
		binding = car(env);

		// If expr equals the name in the (name . value) pair, return
		// the value.
		if (b_equal_pred(car(binding), expr))
		    return cdr(binding);

		env = cdr(env);
	    }
	    env_list = cdr(env_list);
	}

	// env_list doesn't contain a binding for expr, so look it up in the
	// global env.
	LispObject * global_def = get_def(expr);
	if (global_def == NULL) {
	    INVALID_EXPR;
	    print_obj(expr);
	    printf(" is undefined\n");
	}
	return global_def;
    }

    if (!b_list_pred(expr)) {
	if (!b_pair_pred(expr))
	    FOUND_BUG;
	INVALID_EXPR;
	printf("Cannot evaluate a non-list pair\n");
	return NULL;
    }

    if (b_equal_pred(car(expr), LISP_QUOTE)) {
	if (length(cdr(expr)) != 1) {
	    INVALID_EXPR;
	    print_obj(LISP_QUOTE);
	    printf(" takes 1 argument\n");
	    return NULL;
	}
    	return car(cdr(expr));
    }

    if (b_equal_pred(car(expr), LISP_COND)) {

	// clauses is protected from GC because cdr(expr) is reachable from
	// expr, which is protected from GC as per b_eval's pre.
	LispObject * clauses = cdr(expr);

	LispObject * clause;
	LispObject * bool_val;
	while (!b_null_pred(clauses)) {
	    // clauses' protection from GC protects clause from GC because
	    // car(clauses) is reachable from clauses.
	    clause = car(clauses);

	    if (!b_list_pred(clause)) {
		INVALID_EXPR;
		print_obj(clause);
		printf(" is not a list\n");
		return NULL;
	    }

	    if (length(clause) != 2) {
		INVALID_EXPR;
		print_obj(clause);
		printf(" is not of length 2\n");
		return NULL;
	    }

	    // clause being protected from GC meets b_eval's pre that expr is
	    // protected from GC because car(clause) is reachable from clause;
	    // and b_eval's pre that env_list is protected from GC is still
	    // true here.
	    bool_val = b_eval(car(clause), env_list, false);

	    if (bool_val == NULL)
		return NULL;

	    if (bool_val == LISP_T)
		// clause being protected from GC meets b_eval's pre that expr
		// is protected from GC because car(cdr(clause)) is reachable
		// from clause; and b_eval's pre that env_list is protected
		// from GC is still true here.
		return b_eval(car(cdr(clause)), env_list, false);

	    if (bool_val != LISP_F) {
		INVALID_EXPR;
		printf("Predicate ");
		print_obj(car(clause));
		printf(" evaluates to non-boolean value ");
		print_obj(bool_val);
		printf("\n");
		return NULL;
	    }

	    // The new value of clauses is still protected from GC because
	    // cdr(clauses) is reachable from the old value of clauses.
	    clauses = cdr(clauses);
	}
	return LISP_EMPTY;
    }

    if(b_equal_pred(car(expr), LISP_DEFINE)) {
	if (!toplevel) {
	    INVALID_EXPR;
	    print_obj(LISP_DEFINE);
	    printf(" can only occur at the top level\n");
	    return NULL;
	}

	if (length(cdr(expr)) != 2) {
	    INVALID_EXPR;
	    print_obj(LISP_DEFINE);
	    printf(" takes 2 arguments\n");
	    return NULL;
	}

	LispObject * sym = car(cdr(expr));
	if (!b_symbol_pred(sym)) {
	    INVALID_EXPR;
	    print_obj(sym);
	    printf(" is not a symbol\n");
	    return NULL;
	}

	LispObject * def = b_eval(car(cdr(cdr(expr))), env_list, false);
	if (def == NULL)
	    return NULL;

	bool success = bind(sym, def, false);
	if (!success) {
	    INVALID_EXPR;
	    printf("Cannot redefine ");
	    print_obj(sym);
	    printf("\n");
	}
	return NULL;
    }

    if(b_equal_pred(car(expr), LISP_LAMBDA)) {
	if (length(cdr(expr)) != 2) {
	    INVALID_EXPR;
	    print_obj(LISP_LAMBDA);
	    printf(" takes 2 arguments\n");
	    return NULL;
	}

	LispObject * args_list = car(cdr(expr));
	if (!b_list_pred(args_list)) {
	    INVALID_EXPR;
	    print_obj(args_list);
	    printf(" is not a list\n");
	    return NULL;
	}

	// Check that all argument names are symbols.
	while (!b_null_pred(args_list)) {
	    if (!b_symbol_pred(car(args_list))) {
		INVALID_EXPR;
		print_obj(car(args_list));
		printf(" is not a symbol\n");
		return NULL;
	    }
	    args_list = cdr(args_list);
	}

	// Check for duplicate argument names.
	args_list = car(cdr(expr));
	LispObject * args_compare_list;
	while(!b_null_pred(args_list)) {
	    args_compare_list = cdr(args_list);
	    while (!b_null_pred(args_compare_list)) {
		if (b_equal_pred(car(args_list), car(args_compare_list))) {
		    INVALID_EXPR;
		    printf("Duplicate argument name ");
		    print_obj(car(args_list));
		    printf("\n");
		    return NULL;
		}
		args_compare_list = cdr(args_compare_list);
	    }
	    args_list = cdr(args_list);
	}

	// body is protected from GC because expr is protected from GC by
	// b_eval's pre and car(cdr(cdr(expr))) is reachable from expr.
	LispObject * body = car(cdr(cdr(expr)));
	if (b_pair_pred(body) && !b_list_pred(body)) {
	    INVALID_EXPR;
	    print_obj(body);
	    printf(" is a non-list pair\n");
	    return NULL;
	}

	// b_eval's pre that expr is protected from GC meets get_lambda's pre
	// that its first arg is protected from GC, because car(cdr(expr)) is
	// reachable from expr; body's protection from GC meets get_lambda's
	// pre that body is protected from GC; and b_eval's pre that env_list
	// is protected from GC meets get_lambda's pre that env_list is
	// protected from GC.
	return get_lambda(car(cdr(expr)), body, env_list);
    }

    // expr represents a function application.

    LispObject * func = b_eval(car(expr), env_list, false);

    if (func == NULL)
	return NULL;

    // Protect func from GC that could be triggered by calls to b_eval and/or
    // get_new_env, below.
    push(func);

    LispObject * result;
    bool builtin;

    if (func->type == TYPE_BUILTIN_1 || func->type == TYPE_BOOL_BUILTIN_1
	|| func == LISP_BUILTIN_EVAL) {

	builtin = true;

	if (length(cdr(expr)) != 1) {
	    INVALID_EXPR;
	    print_obj(func);
	    printf(" takes 1 argument\n");
	    pop();  // pop func
	    return NULL;
	}

	LispObject * arg1 = b_eval(car(cdr(expr)), env_list, false);
	if (arg1 == NULL) {
	    pop();  // pop func
	    return NULL;
	}

	if (func->type == TYPE_BUILTIN_1)
	    result = func->b_func_1(arg1);

	else if (func->type == TYPE_BOOL_BUILTIN_1)
	    result = (func->b_bool_func_1(arg1) ? LISP_T : LISP_F);

	else
	    // func == LISP_BUILTIN_EVAL
	    result = b_eval(arg1, LISP_EMPTY, false);
    }
    else if (func->type == TYPE_BUILTIN_2 || func->type == TYPE_BOOL_BUILTIN_2) {
	builtin = true;
	
	if (length(cdr(expr)) != 2) {
	    INVALID_EXPR;
	    print_obj(func);
	    printf(" takes 2 arguments\n");
	    pop();  // pop func
	    return NULL;
	}

	LispObject * arg1 = b_eval(car(cdr(expr)), env_list, false);
	if (arg1 == NULL) {
	    pop();  // pop func;
	    return NULL;
	}

	// Protect arg1 from GC that could be triggered by b_eval'ing the
	// second argument.
	push(arg1);

	LispObject * arg2 = b_eval(car(cdr(cdr(expr))), env_list, false);

	pop(); // pop arg1

	if (arg2 == NULL) {
	    pop();  // pop func
	    return NULL;
	}

	if (func->type == TYPE_BUILTIN_2)
	    result = func->b_func_2(arg1, arg2);
	else
	    // func->type == TYPE_BOOL_BUILTIN_2
	    result = (func->b_bool_func_2(arg1, arg2) ? LISP_T : LISP_F);
    }
    else
	builtin = false;

    if (builtin) {
	pop();  // pop func
	if (result == NULL) {
	    INVALID_EXPR;
	    print_obj(func);
	    printf(" signaled an error\n");
	}
	return result;
    }

    if (func->type != TYPE_LAMBDA) {
	INVALID_EXPR;
	print_obj(func);
	printf(" is not a function\n");
	pop();  // pop func
	return NULL;
    }

    // func is protected from GC, so it meets get_new_env's pre that arg_names
    // is protected from GC, because func->args is reachable from func; and
    // b_eval's pre that expr is protected from GC meets get_new_env's pre that
    // arg_exprs is protected from GC, because cdr(expr) is reachable from
    // expr.
    LispObject * arg_names = func->args;
    LispObject * arg_exprs = cdr(expr);

    long len_arg_names = length(arg_names);
    if (length(arg_exprs) != len_arg_names) {
	INVALID_EXPR;
	print_obj(func);
	printf(" takes %ld argument%s",
	       len_arg_names, (len_arg_names == 1 ? "\n" : "s\n"));
	pop();  // pop func
	return NULL;
    }

    // b_eval's pre that env_list is protected from GC meets get_new_env's pre
    // that env_list is protected from GC.
    LispObject * new_env = get_new_env(arg_names, arg_exprs, env_list);
    if (new_env == NULL) {
	pop();  // pop func
	return NULL;
    }

    LispObject * new_env_list = b_cons(new_env, func->env_list);

    // Meet b_eval's pre that env_list is protected from GC.
    push(new_env_list);

    // func is protected from GC, so it meets b_eval's pre that expr is
    // protected from GC, because func->body is reachable from func.
    result = b_eval(func->body, new_env_list, false);

    pop();  // pop new_env_list
    pop();  // pop func

    return result;

    FOUND_BUG;
}


// ============================================================================
// Private functions
// ============================================================================

// get_new_env
// Get a local environment.
//
// Given a list of argument names and a list of expressions, evaluate each
// expression and bind its value to the corresponding name. Return a local
// environment of the form required by b_eval's pre.
//
// Pre:
// - arg_names, arg_exprs, and env_list are protected from garbage collection.
// - arg_names is the empty list or a list of symbols.
// - arg_exprs is a list of the same length as arg_names.
// - env_list is the current list of local environments, of the same form as
//   described by b_eval's pre.
//
// On error:
// - Return NULL.
LispObject * get_new_env(LispObject * arg_names,
			 LispObject * arg_exprs,
			 LispObject * env_list)
{
    LispObject * new_env = LISP_EMPTY;
    LispObject * binding;
    LispObject * arg_val;

    while (!b_null_pred(arg_names)) {

	// Protect new_env from GC that could be triggered by b_eval or b_cons.
	push(new_env);

	// get_new_env's pre that arg_exprs is protected from GC meets b_eval's
	// pre that its first arg is protected from GC, because
	// car(arg_exprs) is reachable from arg_exprs; and get_new_env's pre
	// that env_list is protected from GC meets b_eval's pre that env_list
	// is protected from GC.
	arg_val = b_eval(car(arg_exprs), env_list, false);

	if (arg_val == NULL) {
	    pop();  // pop new_env
	    return NULL;
	}

	// Construct a (name . value) pair.
	binding = b_cons(car(arg_names), arg_val);

	pop();  // pop new_env

	new_env = b_cons(binding, new_env);

	arg_names = cdr(arg_names);
	arg_exprs = cdr(arg_exprs);
    }

    return new_env;
}

