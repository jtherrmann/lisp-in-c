// obj.c
// Source for functions related to Lisp objects.


#include <stdio.h>

#include "obj.h"
#include "env.h"
#include "eval.h"
#include "gc.h"
#include "misc.h"
#include "print.h"
#include "stack.h"


// ============================================================================
// Private functions
// ============================================================================
//
// Functions not declared in header.

LispObject * get_obj(LispType type) {
    // TODO: determine good number for demo; define it in gc.h
    if (weakrefs_count > 50)
	collect_garbage();

    // TODO: check for malloc error code?
    LispObject * obj = malloc(sizeof(LispObject));

    obj->type = type;
    obj->marked = false;

    obj->weakref = weakrefs_head;
    weakrefs_head = obj;
    ++weakrefs_count;

    return obj;
}

LispObject * get_nil() {
    LispObject * obj = get_obj(TYPE_UNIQUE);
    obj->car = obj;
    obj->cdr = obj;
    return obj;
}


// ============================================================================
// LispObject
// ============================================================================

// make_initial_objs
// Construct an initial set of Lisp objects.
//
// This function must be called exactly once. Garbage collection must not be
// triggered for the first time until after this function is called.
void make_initial_objs() {
    LISP_NIL = get_nil();

    LISP_T = get_obj(TYPE_UNIQUE);
    LISP_F = get_obj(TYPE_UNIQUE);

    char T_str[] = "#t";
    LISP_T_SYM = get_sym(T_str);

    char F_str[] = "#f";
    LISP_F_SYM = get_sym(F_str);

    char quote[] = "quote";
    LISP_QUOTE = get_sym(quote);

    char cond[] = "cond";
    LISP_COND = get_sym(cond);

    char def[] = "def";
    LISP_DEF = get_sym(def);

    char lambda[] = "lambda";
    LISP_LAMBDA = get_sym(lambda);

    char eval_str[] = "eval";
    LispObject * eval_name = get_sym(eval_str);
    LispObject * eval_def = get_builtin_1_env(eval_name, &b_eval);
    bind(eval_name, eval_def);

    char cons_str[] = "cons";
    LispObject * cons_name = get_sym(cons_str);
    LispObject * cons_def = get_builtin_2(cons_name, &b_cons);
    bind(cons_name, cons_def);

    char car_str[] = "car";
    LispObject * car_name = get_sym(car_str);
    LispObject * car_def = get_builtin_1(car_name, &b_car_2);
    bind(car_name, car_def);

    char cdr_str[] = "cdr";
    LispObject * cdr_name = get_sym(cdr_str);
    LispObject * cdr_def = get_builtin_1(cdr_name, &b_cdr_2);
    bind(cdr_name, cdr_def);

    char equal_str[] = "eq";
    LispObject * equal_name = get_sym(equal_str);
    LispObject * equal_def = get_bool_builtin_2(equal_name, &b_equal);
    bind(equal_name, equal_def);

    char null_pred_str[] = "null?";
    LispObject * null_pred_name = get_sym(null_pred_str);
    LispObject * null_pred_def = get_bool_builtin_1(null_pred_name, &b_null_pred);
    bind(null_pred_name, null_pred_def);

    char number_pred_str[] = "number?";
    LispObject * number_pred_name = get_sym(number_pred_str);
    LispObject * number_pred_def = get_bool_builtin_1(number_pred_name, &b_number_pred);
    bind(number_pred_name, number_pred_def);

    char symbol_pred_str[] = "symbol?";
    LispObject * symbol_pred_name = get_sym(symbol_pred_str);
    LispObject * symbol_pred_def = get_bool_builtin_1(symbol_pred_name, &b_symbol_pred);
    bind(symbol_pred_name, symbol_pred_def);

    char cons_pred_str[] = "cons?";
    LispObject * cons_pred_name = get_sym(cons_pred_str);
    LispObject * cons_pred_def = get_bool_builtin_1(cons_pred_name, &b_cons_pred);
    bind(cons_pred_name, cons_pred_def);

    char list_pred_str[] = "list?";
    LispObject * list_pred_name = get_sym(list_pred_str);
    LispObject * list_pred_def = get_bool_builtin_1(list_pred_name, &b_list_pred);
    bind(list_pred_name, list_pred_def);

    char func_pred_str[] = "func?";
    LispObject * func_pred_name = get_sym(func_pred_str);
    LispObject * func_pred_def = get_bool_builtin_1(func_pred_name, &b_func_pred);
    bind(func_pred_name, func_pred_def);

    char builtin_pred_str[] = "builtin?";
    LispObject * builtin_pred_name = get_sym(builtin_pred_str);
    LispObject * builtin_pred_def = get_bool_builtin_1(builtin_pred_name, &b_builtin_pred);
    bind(builtin_pred_name, builtin_pred_def);
}


// ----------------------------------------------------------------------------
// Internal constructors
// ----------------------------------------------------------------------------

LispObject * get_int(int value) {
    LispObject * obj = get_obj(TYPE_INT);
    obj->value = value;
    return obj;
}


LispObject * get_sym(char * str) {
    int len = 0;
    while (str[len] != '\0')
	++len;

    LispObject * obj = get_obj(TYPE_SYM);
    obj->print_name = malloc((len + 1) * sizeof(char));

    for (int i = 0; i < len; ++i)
	obj->print_name[i] = str[i];
    obj->print_name[len] = '\0';
    
    return obj;
}


LispObject * get_sym_by_substr(char * str, int begin, int end) {
    int len = end - begin;

    LispObject * obj = get_obj(TYPE_SYM);
    obj->print_name = malloc((len + 1) * sizeof(char));

    for (int i = 0; i < len; ++i)
	obj->print_name[i] = str[begin + i];
    obj->print_name[len] = '\0';

    return obj;
}


// get_func
// Construct a Lisp function.
//
// Pre:
// - args and body are protected from garbage collection.
// - args is the empty list or a list of symbols.
LispObject * get_func(LispObject * args, LispObject * body) {
    LispObject * obj = get_obj(TYPE_FUNC);
    obj->args = args;
    obj->body = body;

    return obj;
}


// get_builtin_1
// Construct a builtin function that takes one argument.
LispObject * get_builtin_1(LispObject * builtin_name,
			   LispObject * (* b_func_1)(LispObject *)) {
    if (!b_symbol_pred(builtin_name))
	FOUND_BUG;
    LispObject * obj = get_obj(TYPE_BUILTIN_1);
    obj->b_func_1 = b_func_1;
    obj->builtin_name = builtin_name;
    return obj;
}


// get_builtin_2
// Construct a builtin function that takes two arguments.
LispObject * get_builtin_2(LispObject * builtin_name,
			   LispObject * (* b_func_2)(LispObject *, LispObject *)) {
    if (!b_symbol_pred(builtin_name))
	FOUND_BUG;
    LispObject * obj = get_obj(TYPE_BUILTIN_2);
    obj->b_func_2 = b_func_2;
    obj->builtin_name = builtin_name;
    return obj;
}


// get_bool_builtin_1
// Construct a builtin function that takes one argument and returns a bool.
LispObject * get_bool_builtin_1(LispObject * builtin_name,
				bool (* b_bool_func_1)(LispObject *)) {
    if (!b_symbol_pred(builtin_name))
	FOUND_BUG;
    LispObject * obj = get_obj(TYPE_BOOL_BUILTIN_1);
    obj->b_bool_func_1 = b_bool_func_1;
    obj->builtin_name = builtin_name;
    return obj;
}


// get_bool_builtin_2
// Construct a builtin function that takes two arguments and returns a bool.
LispObject * get_bool_builtin_2(LispObject * builtin_name,
				bool (* b_bool_func_2)(LispObject *, LispObject *)) {
    if (!b_symbol_pred(builtin_name))
	FOUND_BUG;
    LispObject * obj = get_obj(TYPE_BOOL_BUILTIN_2);
    obj->b_bool_func_2 = b_bool_func_2;
    obj->builtin_name = builtin_name;
    return obj;
}


// get_builtin_1_env
// Construct a builtin function that takes one explicit argument and one
// implicit argument: the local environment.
LispObject * get_builtin_1_env(LispObject * builtin_name,
			       LispObject * (* b_func_1_env)(LispObject *, LispObject *)) {
    if (!b_symbol_pred(builtin_name))
	FOUND_BUG;
    LispObject * obj = get_obj(TYPE_BUILTIN_1_ENV);
    obj->b_func_1_env = b_func_1_env;
    obj->builtin_name = builtin_name;
    return obj;
}


// ----------------------------------------------------------------------------
// cons, car, and cdr
// ----------------------------------------------------------------------------

// b_cons
// Builtin Lisp function cons.
LispObject * b_cons(LispObject * car, LispObject * cdr) {
    // Protect car and cdr from GC that could be triggered by get_obj.
    push(car);
    push(cdr);

    LispObject * obj = get_obj(TYPE_CONS);

    pop();
    pop();

    obj->car = car;
    obj->cdr = cdr;

    return obj;
}


// TODO: rename the car/cdr funcs


// b_car_2
// Builtin Lisp function car.
LispObject * b_car_2(LispObject * obj) {
    if (!b_list_pred(obj)) {
	printf("Type error: ");
	print_obj(obj);
	printf(" is not a list\n\n");
	return NULL;
    }
    return obj->car;
}


// b_cdr_2
// Builtin Lisp function cdr.
LispObject * b_cdr_2(LispObject * obj) {
    if (!b_list_pred(obj)) {
	printf("Type error: ");
	print_obj(obj);
	printf(" is not a list\n\n");
	return NULL;
    }
    return obj->cdr;
}


// b_car
// Previously this was builtin Lisp function car, but now it is just for
// internal use by the interpreter, while b_car_2 implements a builtin car
// function with proper error handling.
LispObject * b_car(LispObject * obj) {
    if (!b_list_pred(obj))
	FOUND_BUG;
    return obj->car;
}


// b_cdr
// Previously this was builtin Lisp function cdr, but now it is just for
// internal use by the interpreter, while b_cdr_2 implements a builtin cdr
// function with proper error handling.
LispObject * b_cdr(LispObject * obj) {
    if (!b_list_pred(obj))
	FOUND_BUG;
    return obj->cdr;
}


// ============================================================================
// Type predicates
// ============================================================================

bool b_null_pred(LispObject * obj) {
    return obj == LISP_NIL;
}


bool b_number_pred(LispObject * obj) {
    return obj->type == TYPE_INT;
}


bool b_symbol_pred(LispObject * obj) {
    return obj->type == TYPE_SYM;
}


bool b_cons_pred(LispObject * obj) {
    return obj->type == TYPE_CONS;
}


bool b_list_pred(LispObject * obj) {
    return b_cons_pred(obj) || b_null_pred(obj);
}


bool b_func_pred(LispObject * obj) {
    return obj->type == TYPE_FUNC;
}


bool b_builtin_pred(LispObject * obj) {
    return obj->type == TYPE_BUILTIN_1
	|| obj->type == TYPE_BUILTIN_2
	|| obj->type == TYPE_BOOL_BUILTIN_1
	|| obj->type == TYPE_BOOL_BUILTIN_2
	|| obj->type == TYPE_BUILTIN_1_ENV;
}


// ============================================================================
// Comparison functions
// ============================================================================

bool b_equal(LispObject * obj1, LispObject * obj2) {
    if (obj1 == obj2)
	return true;

    if (obj1->type != obj2->type)
	return false;

    if (obj1->type == TYPE_UNIQUE)
	return obj1 == obj2;

    if (b_number_pred(obj1))
	return obj1->value == obj2->value;

    if (b_symbol_pred(obj1)) {
	// TODO: once string interning implemented, just compare str ptrs (or
	// intern entire symbols, in which case remove this if block because
	// the first if (obj1 == obj2) will execute)
	int i = 0;
	while (obj1->print_name[i] != '\0' && obj2->print_name[i] != '\0') {
	    if (obj1->print_name[i] != obj2->print_name[i])
		return false;
	    ++i;
	}
	return obj1->print_name[i] == obj2->print_name[i];
    }

    if (b_cons_pred(obj1))
	return b_equal(b_car(obj1), b_car(obj2))
	    && b_equal(b_cdr(obj1), b_cdr(obj2));

    FOUND_BUG;
    exit(1);  // Avoid gcc warning about missing return.
}
