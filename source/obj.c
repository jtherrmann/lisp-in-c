// obj.c
// Source for functions related to Lisp objects.


#include <stdio.h>

#include "obj.h"
#include "builtins.h"
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

LispObject * get_builtin_0(char * name_str, LispObject * (* b_func_0)());

LispObject * get_builtin_1(char * name_str,
			   LispObject * (* b_func_1)(LispObject *));

LispObject * get_builtin_2(char * name_str,
			   LispObject * (* b_func_2)(LispObject *, LispObject *));

LispObject * get_bool_builtin_1(char * name_str,
				bool (* b_bool_func_1)(LispObject *));

LispObject * get_bool_builtin_2(char * name_str,
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

    LISP_T = get_sym("t");
    bind(LISP_T, LISP_T, true);

    LISP_QUOTE = get_sym("quote");
    LISP_COND = get_sym("cond");
    LISP_DEFINE = get_sym("define");
    LISP_LAMBDA = get_sym("lambda");

    get_builtin_1("eval", &b_eval);
    get_builtin_2("cons", &b_cons);
    get_builtin_1("car", &b_car);
    get_builtin_1("cdr", &b_cdr);
    get_builtin_1("length", &b_length);

    get_builtin_2("+", &b_add);
    get_builtin_2("-", &b_sub);
    get_builtin_2("*", &b_mul);
    get_builtin_2("/", &b_div);

    get_bool_builtin_2("equal?", &b_equal_pred);
    get_builtin_2("<", &b_lt);

    get_bool_builtin_1("null?", &b_null_pred);
    get_bool_builtin_1("symbol?", &b_symbol_pred);
    get_bool_builtin_1("function?", &b_function_pred);

    char int_pred_str[] = "int?";
    get_bool_builtin_1(int_pred_str, &b_int_pred);
    LISP_INT_PRED_SYM = get_sym(int_pred_str);

    char pair_pred_str[] = "pair?";
    get_bool_builtin_1(pair_pred_str, &b_pair_pred);
    LISP_PAIR_PRED_SYM = get_sym(pair_pred_str);

    char list_pred_str[] = "list?";
    get_bool_builtin_1(list_pred_str, &b_list_pred);
    LISP_LIST_PRED_SYM = get_sym(list_pred_str);

    get_builtin_0("print-weakrefs", &b_print_weakrefs);
    get_builtin_1("print-env", &b_print_env);

    LISP_GC_OUTPUT = get_sym("gc-output");
    bind(LISP_GC_OUTPUT, LISP_EMPTY, false);

    LISP_STACK_OUTPUT = get_sym("stack-output");
    bind(LISP_STACK_OUTPUT, LISP_EMPTY, false);
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
//   described by eval's pre.
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
    // TODO: for now we just invoke GC when the total number of objects exceeds
    // some value, but there are certainly better ways to do it
    if (weakrefs_count > 1000)
	collect_garbage();

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


// get_builtin_0
// Construct a builtin function that takes no arguments.
LispObject * get_builtin_0(char * name_str,
			   LispObject * (* b_func_0)()) {
    LispObject * name = get_sym(name_str);
    LispObject * obj = get_obj(TYPE_BUILTIN_0);
    obj->b_func_0 = b_func_0;
    obj->builtin_name = name;
    bind(name, obj, true);
    return obj;
}


// get_builtin_1
// Construct a builtin function that takes one argument.
LispObject * get_builtin_1(char * name_str,
			   LispObject * (* b_func_1)(LispObject *)) {
    LispObject * name = get_sym(name_str);
    LispObject * obj = get_obj(TYPE_BUILTIN_1);
    obj->b_func_1 = b_func_1;
    obj->builtin_name = name;
    bind(name, obj, true);
    return obj;
}


// get_builtin_2
// Construct a builtin function that takes two arguments.
LispObject * get_builtin_2(char * name_str,
			   LispObject * (* b_func_2)(LispObject *, LispObject *)) {
    LispObject * name = get_sym(name_str);
    LispObject * obj = get_obj(TYPE_BUILTIN_2);
    obj->b_func_2 = b_func_2;
    obj->builtin_name = name;
    bind(name, obj, true);
    return obj;
}


// get_bool_builtin_1
// Construct a builtin function that takes one argument and returns a bool.
LispObject * get_bool_builtin_1(char * name_str,
				bool (* b_bool_func_1)(LispObject *)) {
    LispObject * name = get_sym(name_str);
    LispObject * obj = get_obj(TYPE_BOOL_BUILTIN_1);
    obj->b_bool_func_1 = b_bool_func_1;
    obj->builtin_name = name;
    bind(name, obj, true);
    return obj;
}


// get_bool_builtin_2
// Construct a builtin function that takes two arguments and returns a bool.
LispObject * get_bool_builtin_2(char * name_str,
				bool (* b_bool_func_2)(LispObject *, LispObject *)) {
    LispObject * name = get_sym(name_str);
    LispObject * obj = get_obj(TYPE_BOOL_BUILTIN_2);
    obj->b_bool_func_2 = b_bool_func_2;
    obj->builtin_name = name;
    bind(name, obj, true);
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
    ASSERT(b_pair_pred(obj));
    return obj->car;
}


// cdr
// cdr function for internal use by the interpreter.
LispObject * cdr(LispObject * obj) {
    ASSERT(b_pair_pred(obj));
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


long length(LispObject * obj) {
    ASSERT(b_list_pred(obj));
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
    return obj->type == TYPE_BUILTIN_0
	|| obj->type == TYPE_BUILTIN_1
	|| obj->type == TYPE_BUILTIN_2
	|| obj->type == TYPE_BOOL_BUILTIN_1
	|| obj->type == TYPE_BOOL_BUILTIN_2;
}


// ============================================================================
// Miscellaneous utilities
// ============================================================================

bool get_config_bool(LispObject * obj) {
    ASSERT(b_equal_pred(obj, LISP_STACK_OUTPUT)
	   || b_equal_pred(obj, LISP_GC_OUTPUT));

    LispObject * def = get_def(obj);
    ASSERT(def != NULL);
    return to_bool(def);
}


bool to_bool(LispObject * obj) {
    return (b_null_pred(obj) ? false : true);
}
