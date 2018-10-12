// obj.c
// Source for functions related to Lisp objects.


#include <assert.h>

#include "obj.h"
#include "gc.h"


// ============================================================================
// Private functions
// ============================================================================
//
// Functions not declared in header.

LispObject * get_obj(LispType type) {
    LispObject * obj = malloc(sizeof(LispObject));
    // TODO: check for malloc error code?
    obj->type = type;
    obj->weakref = weakrefs_head;
    weakrefs_head = obj;
    return obj;
}

LispObject * get_nil() {
    LispObject * obj = get_obj(LISP_NILTYPE);
    obj->car = obj;
    obj->cdr = obj;
    return obj;
}


// ============================================================================
// LispObject
// ============================================================================

// TODO: these objs need to be marked during GC--or maybe just don't add them
// to weakrefs list so they can't be swept; easy way to do this would be to set
// weakrefs_head to NULL in main AFTER calling this func
//
// or maybe just don't even bother with any of these except for LISP_NIL
LispObject * make_initial_objs() {
    LISP_NIL = get_nil();

    char str[] = "quote";
    LISP_QUOTE = get_sym(str);
}


// ----------------------------------------------------------------------------
// Internal constructors
// ----------------------------------------------------------------------------

LispObject * get_int(int value) {
    LispObject * obj = get_obj(LISP_INT);
    obj->value = value;
}


LispObject * get_sym(char * str) {
    int len = 0;
    while (str[len] != '\0')
	++len;

    LispObject * obj = get_obj(LISP_SYM);
    obj->print_name = malloc((len + 1) * sizeof(char));

    for (int i = 0; i < len; ++i)
	obj->print_name[i] = str[i];
    obj->print_name[len] = '\0';
    
    return obj;
}


LispObject * get_sym_by_substr(char * str, int begin, int end) {
    int len = end - begin;

    LispObject * obj = get_obj(LISP_SYM);
    obj->print_name = malloc((len + 1) * sizeof(char));

    for (int i = 0; i < len; ++i)
	obj->print_name[i] = str[begin + i];
    obj->print_name[len] = '\0';

    return obj;
}


// ----------------------------------------------------------------------------
// cons, car, and cdr
// ----------------------------------------------------------------------------

LispObject * b_cons(LispObject * car, LispObject * cdr) {
    LispObject * obj = get_obj(LISP_CONS);
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
    return obj->type == LISP_INT;
}


bool b_symbolp(LispObject * obj) {
    return obj->type == LISP_SYM;
}


bool b_consp(LispObject * obj) {
    return obj->type == LISP_CONS;
}


bool b_listp(LispObject * obj) {
    return b_consp(obj) || b_null(obj);
}

