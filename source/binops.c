// binops.c
// Source for builtin binary operators.

#include <stdio.h>

#include "binops.h"
#include "error.h"
#include "print.h"


// ============================================================================
// Boolean logic
// ============================================================================

// b_and
// Builtin Lisp function and.
LispObject * b_and(LispObject * obj1, LispObject * obj2) {
    if (!typecheck(obj1, LISP_BOOL_PRED_SYM)
	|| !typecheck(obj2, LISP_BOOL_PRED_SYM))
	return NULL;
    return (obj1 == LISP_T && obj2 == LISP_T ? LISP_T : LISP_F);
}


// b_or
// Builtin Lisp function or.
LispObject * b_or(LispObject * obj1, LispObject * obj2) {
    if (!typecheck(obj1, LISP_BOOL_PRED_SYM)
	|| !typecheck(obj2, LISP_BOOL_PRED_SYM))
	return NULL;
    return (obj1 == LISP_T || obj2 == LISP_T ? LISP_T : LISP_F);
}
    

// b_not
// Builtin Lisp function not.
LispObject * b_not(LispObject * obj) {
    if (!typecheck(obj, LISP_BOOL_PRED_SYM))
	return NULL;
    return (obj == LISP_T ? LISP_F : LISP_T);
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