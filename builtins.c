#include "builtins.h"

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

