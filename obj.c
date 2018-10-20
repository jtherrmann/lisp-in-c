// obj.c
// Source for functions related to Lisp objects.


#include <assert.h>
#include <stdio.h>

#include "obj.h"
#include "env.h"
#include "gc.h"
#include "stack.h"


// ============================================================================
// Private functions
// ============================================================================
//
// Functions not declared in header.

LispObject * get_obj(LispType type) {
    // TODO: determine good number for demo; define it in gc.h
    if (weakrefs_count > 20)
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
    LispObject * obj = get_obj(TYPE_NIL);
    obj->car = obj;
    obj->cdr = obj;
    return obj;
}


// ============================================================================
// LispObject
// ============================================================================

// make_initial_objs
// Construct an initial set of Lisp objects.
void make_initial_objs() {
    LISP_NIL = get_nil();

    char quote[] = "quote";
    LISP_QUOTE = get_sym(quote);

    char def[] = "def";
    LISP_DEF = get_sym(def);

    char lambda[] = "lambda";
    LISP_LAMBDA = get_sym(lambda);

    char cons_str[] = "cons";
    LispObject * cons_name = get_sym(cons_str);
    LispObject * cons_def = get_builtin_2(&b_cons);
    bind(cons_name, cons_def);

    char car_str[] = "car";
    LispObject * car_name = get_sym(car_str);
    LispObject * car_def = get_builtin_1(&b_car);
    bind(car_name, car_def);

    char cdr_str[] = "cdr";
    LispObject * cdr_name = get_sym(cdr_str);
    LispObject * cdr_def = get_builtin_1(&b_cdr);
    bind(cdr_name, cdr_def);
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
// Pre: args and body are protected from garbage collection.
LispObject * get_func(LispObject * args, LispObject * body) {
    // TODO: proper errors
    assert(b_listp(args));

    LispObject * obj = get_obj(TYPE_FUNC);
    obj->args = args;
    obj->body = body;

    return obj;
}


// get_builtin_1
// Construct a builtin function that takes one argument.
LispObject * get_builtin_1(LispObject * (* b_func_1)(LispObject *)) {
    LispObject * obj = get_obj(TYPE_BUILTIN_1);
    obj->b_func_1 = b_func_1;
    return obj;
}


// get_builtin_2
// Construct a builtin function that takes two arguments.
LispObject * get_builtin_2(LispObject * (* b_func_2)(LispObject *, LispObject *)) {
    LispObject * obj = get_obj(TYPE_BUILTIN_2);
    obj->b_func_2 = b_func_2;
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


LispObject * b_car(LispObject * obj) {
    // TODO: proper typecheck
    assert(b_listp(obj));
    return obj->car;
}


LispObject * b_cdr(LispObject * obj) {
    // TODO: proper typecheck
    assert(b_listp(obj));
    return obj->cdr;
}


// ============================================================================
// Type predicates
// ============================================================================

bool b_null(LispObject * obj) {
    return obj == LISP_NIL;
}


bool b_numberp(LispObject * obj) {
    return obj->type == TYPE_INT;
}


bool b_symbolp(LispObject * obj) {
    return obj->type == TYPE_SYM;
}


bool b_consp(LispObject * obj) {
    return obj->type == TYPE_CONS;
}


bool b_listp(LispObject * obj) {
    return b_consp(obj) || b_null(obj);
}


bool b_funcp(LispObject * obj) {
    return obj->type == TYPE_FUNC;
}


bool b_builtinp(LispObject * obj) {
    return obj->type == TYPE_BUILTIN_1 || obj->type == TYPE_BUILTIN_2;
}


// ============================================================================
// Comparison functions
// ============================================================================

bool b_equal(LispObject * obj1, LispObject * obj2) {
    if (obj1 == obj2)
	return true;

    if (obj1->type != obj2->type)
	return false;

    if (b_numberp(obj1))
	return obj1->value == obj2->value;

    if (b_symbolp(obj1)) {
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

    if (b_consp(obj1))
	return b_equal(b_car(obj1), b_car(obj2))
	    && b_equal(b_cdr(obj1), b_cdr(obj2));

    printf("COMPARISON ERROR: unrecognized type\n");
    exit(1);
}
