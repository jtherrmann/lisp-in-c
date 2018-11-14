// obj.c
// Source for functions related to Lisp objects.


#include <stdio.h>

#include "obj.h"
#include "binops.h"
#include "env.h"
#include "eval.h"
#include "gc.h"
#include "error.h"
#include "print.h"
#include "stack.h"


// ============================================================================
// Private function prototypes
// ============================================================================

LispObject * get_obj(LispType type);

LispObject * get_empty_list();

LispObject * get_sym(char * str);

LispObject * get_builtin_eval(LispObject * builtin_name);

LispObject * get_builtin_1(LispObject * builtin_name,
			   LispObject * (* b_func_1)(LispObject *));

LispObject * get_builtin_2(LispObject * builtin_name,
			   LispObject * (* b_func_2)(LispObject *, LispObject *));

LispObject * get_bool_builtin_1(LispObject * builtin_name,
				bool (* b_bool_func_1)(LispObject *));

LispObject * get_bool_builtin_2(LispObject * builtin_name,
				bool (* b_bool_func_2)(LispObject *, LispObject *));

// ============================================================================
// LispObject
// ============================================================================

// make_initial_objs
// Construct an initial set of Lisp objects.
//
// This function must be called exactly once. Garbage collection must not be
// triggered for the first time until after this function is called.
void make_initial_objs() {
    LISP_EMPTY = get_empty_list();

    LISP_T = get_obj(TYPE_UNIQUE);
    LISP_F = get_obj(TYPE_UNIQUE);

    char quote[] = "quote";
    LISP_QUOTE = get_sym(quote);

    char cond[] = "cond";
    LISP_COND = get_sym(cond);

    char define[] = "define";
    LISP_DEFINE = get_sym(define);

    char lambda[] = "lambda";
    LISP_LAMBDA = get_sym(lambda);

    char eval_str[] = "eval";
    LispObject * eval_name = get_sym(eval_str);
    LISP_BUILTIN_EVAL = get_builtin_eval(eval_name);
    bind(eval_name, LISP_BUILTIN_EVAL, true);

    char cons_str[] = "cons";
    LispObject * cons_name = get_sym(cons_str);
    LispObject * cons_def = get_builtin_2(cons_name, &b_cons);
    bind(cons_name, cons_def, true);

    char car_str[] = "car";
    LispObject * car_name = get_sym(car_str);
    LispObject * car_def = get_builtin_1(car_name, &b_car);
    bind(car_name, car_def, true);

    char cdr_str[] = "cdr";
    LispObject * cdr_name = get_sym(cdr_str);
    LispObject * cdr_def = get_builtin_1(cdr_name, &b_cdr);
    bind(cdr_name, cdr_def, true);

    char length_str[] = "length";
    LispObject * length_name = get_sym(length_str);
    LispObject * length_def = get_builtin_1(length_name, &b_length);
    bind(length_name, length_def, true);

    char add_str[] = "+";
    LispObject * add_name = get_sym(add_str);
    LispObject * add_def = get_builtin_2(add_name, &b_add);
    bind(add_name, add_def, true);

    char sub_str[] = "-";
    LispObject * sub_name = get_sym(sub_str);
    LispObject * sub_def = get_builtin_2(sub_name, &b_sub);
    bind(sub_name, sub_def, true);

    char mul_str[] = "*";
    LispObject * mul_name = get_sym(mul_str);
    LispObject * mul_def = get_builtin_2(mul_name, &b_mul);
    bind(mul_name, mul_def, true);

    char div_str[] = "/";
    LispObject * div_name = get_sym(div_str);
    LispObject * div_def = get_builtin_2(div_name, &b_div);
    bind(div_name, div_def, true);

    char and_str[] = "and";
    LispObject * and_name = get_sym(and_str);
    LispObject * and_def = get_builtin_2(and_name, &b_and);
    bind(and_name, and_def, true);

    char or_str[] = "or";
    LispObject * or_name = get_sym(or_str);
    LispObject * or_def = get_builtin_2(or_name, &b_or);
    bind(or_name, or_def, true);

    char not_str[] = "not";
    LispObject * not_name = get_sym(not_str);
    LispObject * not_def = get_builtin_1(not_name, &b_not);
    bind(not_name, not_def, true);

    char equal_str[] = "equal?";
    LispObject * equal_name = get_sym(equal_str);
    LispObject * equal_def = get_bool_builtin_2(equal_name, &b_equal_pred);
    bind(equal_name, equal_def, true);

    char lt_str[] = "<";
    LispObject * lt_name = get_sym(lt_str);
    LispObject * lt_def = get_builtin_2(lt_name, &b_lt);
    bind(lt_name, lt_def, true);

    char lte_str[] = "<=";
    LispObject * lte_name = get_sym(lte_str);
    LispObject * lte_def = get_builtin_2(lte_name, &b_lte);
    bind(lte_name, lte_def, true);

    char gt_str[] = ">";
    LispObject * gt_name = get_sym(gt_str);
    LispObject * gt_def = get_builtin_2(gt_name, &b_gt);
    bind(gt_name, gt_def, true);

    char gte_str[] = ">=";
    LispObject * gte_name = get_sym(gte_str);
    LispObject * gte_def = get_builtin_2(gte_name, &b_gte);
    bind(gte_name, gte_def, true);

    char null_pred_str[] = "null?";
    LispObject * null_pred_name = get_sym(null_pred_str);
    LispObject * null_pred_def = get_bool_builtin_1(null_pred_name, &b_null_pred);
    bind(null_pred_name, null_pred_def, true);

    char bool_pred_str[] = "bool?";
    LISP_BOOL_PRED_SYM = get_sym(bool_pred_str);
    LispObject * bool_pred_def = get_bool_builtin_1(LISP_BOOL_PRED_SYM, &b_bool_pred);
    bind(LISP_BOOL_PRED_SYM, bool_pred_def, true);

    char int_pred_str[] = "int?";
    LISP_INT_PRED_SYM = get_sym(int_pred_str);
    LispObject * int_pred_def = get_bool_builtin_1(LISP_INT_PRED_SYM, &b_int_pred);
    bind(LISP_INT_PRED_SYM, int_pred_def, true);

    char symbol_pred_str[] = "symbol?";
    LispObject * symbol_pred_name = get_sym(symbol_pred_str);
    LispObject * symbol_pred_def = get_bool_builtin_1(symbol_pred_name, &b_symbol_pred);
    bind(symbol_pred_name, symbol_pred_def, true);

    char pair_pred_str[] = "pair?";
    LISP_PAIR_PRED_SYM = get_sym(pair_pred_str);
    LispObject * pair_pred_def = get_bool_builtin_1(LISP_PAIR_PRED_SYM, &b_pair_pred);
    bind(LISP_PAIR_PRED_SYM, pair_pred_def, true);

    char list_pred_str[] = "list?";
    LISP_LIST_PRED_SYM = get_sym(list_pred_str);
    LispObject * list_pred_def = get_bool_builtin_1(LISP_LIST_PRED_SYM, &b_list_pred);
    bind(LISP_LIST_PRED_SYM, list_pred_def, true);

    char function_pred_str[] = "function?";
    LispObject * function_pred_name = get_sym(function_pred_str);
    LispObject * function_pred_def = get_bool_builtin_1(function_pred_name, &b_function_pred);
    bind(function_pred_name, function_pred_def, true);
}


// ----------------------------------------------------------------------------
// Public constructors
// ----------------------------------------------------------------------------

// get_int
// Construct a Lisp int.
LispObject * get_int(long value) {
    LispObject * obj = get_obj(TYPE_INT);
    obj->value = value;
    return obj;
}


// get_sym_by_substr
// Construct a Lisp symbol from a substr of str.
LispObject * get_sym_by_substr(char * str, long begin, long end) {
    long len = end - begin;

    LispObject * obj = get_obj(TYPE_SYM);
    obj->print_name = malloc((len + 1) * sizeof(char));

    for (long i = 0; i < len; ++i)
	obj->print_name[i] = str[begin + i];
    obj->print_name[len] = '\0';

    return obj;
}


// get_lambda
// Construct a Lisp lambda function.
//
// Pre:
// - args, body, and env_list are protected from garbage collection.
// - args is the empty list or a list of symbols.
// - env_list is the current list of local environments, of the same form as
//   described by b_eval's pre.
LispObject * get_lambda(LispObject * args, LispObject * body, LispObject * env_list) {
    LispObject * obj = get_obj(TYPE_LAMBDA);
    obj->args = args;
    obj->body = body;
    obj->env_list = env_list;

    return obj;
}


// b_cons
// Builtin Lisp function cons.
LispObject * b_cons(LispObject * car, LispObject * cdr) {
    // Protect car and cdr from GC that could be triggered by get_obj.
    push(car);
    push(cdr);

    LispObject * obj = get_obj(TYPE_PAIR);

    pop();
    pop();

    obj->is_list = cdr->is_list;
    obj->car = car;
    obj->cdr = cdr;

    return obj;
}


// ----------------------------------------------------------------------------
// Private constructors
// ----------------------------------------------------------------------------

// get_obj
// Construct a Lisp object.
LispObject * get_obj(LispType type) {
    // TODO: determine good number for demo; define it in gc.h
    if (weakrefs_count > 100)
	collect_garbage();

    // TODO: check for malloc error code?
    LispObject * obj = malloc(sizeof(LispObject));

    obj->type = type;
    obj->is_list = false;
    obj->marked = false;

    obj->weakref = weakrefs_head;
    weakrefs_head = obj;
    ++weakrefs_count;

    return obj;
}


// get_empty_list
// Construct the empty list object.
LispObject * get_empty_list() {
    LispObject * obj = get_obj(TYPE_UNIQUE);
    obj->is_list = true;
    return obj;
}


// get_sym
// Construct a Lisp symbol from str.
LispObject * get_sym(char * str) {
    long len = 0;
    while (str[len] != '\0')
	++len;

    LispObject * obj = get_obj(TYPE_SYM);
    obj->print_name = malloc((len + 1) * sizeof(char));

    for (long i = 0; i < len; ++i)
	obj->print_name[i] = str[i];
    obj->print_name[len] = '\0';
    
    return obj;
}


// get_builtin_eval
// Construct the builtin eval function.
LispObject * get_builtin_eval(LispObject * builtin_name) {
    if (!b_symbol_pred(builtin_name))
	FOUND_BUG;
    LispObject * obj = get_obj(TYPE_UNIQUE);
    obj->builtin_name = builtin_name;
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


// ============================================================================
// car, cdr, and length
// ============================================================================

// b_car
// Builtin Lisp function car.
LispObject * b_car(LispObject * obj) {
    if (!typecheck(obj, LISP_PAIR_PRED_SYM))
    	return NULL;
    return obj->car;
}


// b_cdr
// Builtin Lisp function cdr.
LispObject * b_cdr(LispObject * obj) {
    if (!typecheck(obj, LISP_PAIR_PRED_SYM))
    	return NULL;
    return obj->cdr;
}


// car
// car function for internal use by the interpreter.
LispObject * car(LispObject * obj) {
    if (!b_pair_pred(obj))
	FOUND_BUG;
    return obj->car;
}


// cdr
// cdr function for internal use by the interpreter.
LispObject * cdr(LispObject * obj) {
    if (!b_pair_pred(obj))
	FOUND_BUG;
    return obj->cdr;
}


// b_length
// Builtin Lisp function length.
LispObject * b_length(LispObject * obj) {
    if (!typecheck(obj, LISP_LIST_PRED_SYM))
	return NULL;

    push(obj);  // Protect obj from GC that could be triggered by get_int.
    LispObject * result = get_int(length(obj));
    pop();
    return result;
}


// length
// Return the length of a Lisp list.
//
// Pre:
// - b_list_pred(obj)
long length(LispObject * obj) {
    long count = 0;
    while (!b_null_pred(obj)) {
	++count;
	obj = cdr(obj);
    }
    return count;
}


// ============================================================================
// Type predicates
// ============================================================================

// b_null_pred
// Builtin Lisp function null?.
bool b_null_pred(LispObject * obj) {
    return obj == LISP_EMPTY;
}


// b_bool_pred
// Builtin Lisp function bool?.
bool b_bool_pred(LispObject * obj) {
    return obj == LISP_T || obj == LISP_F;
}


// b_int_pred
// Builtin Lisp function int?.
bool b_int_pred(LispObject * obj) {
    return obj->type == TYPE_INT;
}


// b_symbol_pred
// Builtin Lisp function symbol?.
bool b_symbol_pred(LispObject * obj) {
    return obj->type == TYPE_SYM;
}


// b_pair_pred
// Builtin Lisp function pair?.
bool b_pair_pred(LispObject * obj) {
    return obj->type == TYPE_PAIR;
}


// b_list_pred
// Builtin Lisp function list?.
bool b_list_pred(LispObject * obj) {
    return obj->is_list;
}


// b_function_pred
// Builtin Lisp function function?.
bool b_function_pred(LispObject * obj) {
    return obj->type == TYPE_LAMBDA || is_builtin(obj);
}


// is_builtin
// Return whether the object is a builtin Lisp function.
bool is_builtin(LispObject * obj) {
    return obj->type == TYPE_BUILTIN_1
	|| obj->type == TYPE_BUILTIN_2
	|| obj->type == TYPE_BOOL_BUILTIN_1
	|| obj->type == TYPE_BOOL_BUILTIN_2
	|| obj == LISP_BUILTIN_EVAL;
}

