// builtins.c
// Source for Lisp builtins.


#include <assert.h>

#include "builtins.h"
#include "gc.h"


// ============================================================================
// Private functions
// ============================================================================
//
// Functions not declared in header.

LispObject * _lisp_obj(LispType type) {
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

LispObject * lisp_nil() {
    LispObject * obj = _lisp_obj(LISP_NILTYPE);
    obj->car = obj;
    obj->cdr = obj;
    return obj;
}


LispObject * lisp_int(int value) {
    LispObject * obj = _lisp_obj(LISP_INT);
    obj->value = value;
}


// ----------------------------------------------------------------------------
// cons, car, and cdr
// ----------------------------------------------------------------------------

LispObject * lisp_cons(LispObject * car, LispObject * cdr) {
    LispObject * obj = _lisp_obj(LISP_CONS);
    obj->car = car;
    obj->cdr = cdr;
    return obj;
}


LispObject * lisp_car(LispObject * obj) {
    // TODO: proper typecheck
    assert(lisp_listp(obj));
    return obj->car;
}


LispObject * lisp_cdr(LispObject * obj) {
    // TODO: proper typecheck
    assert(lisp_listp(obj));
    return obj->cdr;
}


// ============================================================================
// Type predicates
// ============================================================================

bool lisp_null(LispObject * object) {
    return object == LISP_NIL;
}


bool lisp_numberp(LispObject * object) {
    return object->type == LISP_INT;
}


bool lisp_consp(LispObject * object) {
    return object->type == LISP_CONS;
}


bool lisp_listp(LispObject * object) {
    return lisp_consp(object) || lisp_null(object);
}

