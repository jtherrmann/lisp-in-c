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


// ============================================================================
// LispObject
// ============================================================================

// ----------------------------------------------------------------------------
// Internal constructors
// ----------------------------------------------------------------------------

LispObject * get_nil() {
    LispObject * obj = get_obj(LISP_NILTYPE);
    obj->car = obj;
    obj->cdr = obj;
    return obj;
}


LispObject * get_int(int value) {
    LispObject * obj = get_obj(LISP_INT);
    obj->value = value;
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

