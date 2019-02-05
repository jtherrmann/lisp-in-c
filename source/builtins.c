// builtins.c
// Source for miscellaneous builtin functions.


#include <stdio.h>

#include "builtins.h"
#include "error.h"
#include "print.h"
#include "stack.h"


// ============================================================================
// Arithmetic
// ============================================================================

// b_add
// Builtin Lisp function +.
LispObject * b_add(LispObject * obj1, LispObject * obj2) {
    if (!typecheck(obj1, LISP_INT_PRED_SYM)
	|| !typecheck(obj2, LISP_INT_PRED_SYM))
	return NULL;

    // Protect operands from GC that could be triggered by get_int.
    push(obj1);
    push(obj2);

    LispObject * result = get_int(obj1->value + obj2->value);

    pop();
    pop();

    return result;
}


// b_sub
// Builtin Lisp function -.
LispObject * b_sub(LispObject * obj1, LispObject * obj2) {
    if (!typecheck(obj1, LISP_INT_PRED_SYM)
	|| !typecheck(obj2, LISP_INT_PRED_SYM))
	return NULL;

    // Protect operands from GC that could be triggered by get_int.
    push(obj1);
    push(obj2);

    LispObject * result = get_int(obj1->value - obj2->value);

    pop();
    pop();

    return result;
}


// b_mul
// Builtin Lisp function *.
LispObject * b_mul(LispObject * obj1, LispObject * obj2) {
    if (!typecheck(obj1, LISP_INT_PRED_SYM)
	|| !typecheck(obj2, LISP_INT_PRED_SYM))
	return NULL;

    // Protect operands from GC that could be triggered by get_int.
    push(obj1);
    push(obj2);

    LispObject * result = get_int(obj1->value * obj2->value);

    pop();
    pop();

    return result;
}


// TODO: error for divide by zero
//
// b_div
// Builtin Lisp function /.
LispObject * b_div(LispObject * obj1, LispObject * obj2) {
    if (!typecheck(obj1, LISP_INT_PRED_SYM)
	|| !typecheck(obj2, LISP_INT_PRED_SYM))
	return NULL;

    // Protect operands from GC that could be triggered by get_int.
    push(obj1);
    push(obj2);

    LispObject * result = get_int(obj1->value / obj2->value);

    pop();
    pop();

    return result;
}


// ============================================================================
// Comparison functions
// ============================================================================

// b_equal_pred
// Builtin Lisp function equal?.
bool b_equal_pred(LispObject * obj1, LispObject * obj2) {
    if (obj1 == obj2)
	return true;

    if (obj1->type != obj2->type)
	return false;

    if (b_int_pred(obj1))
	return obj1->value == obj2->value;

    if (b_symbol_pred(obj1)) {
	long i = 0;
	while (obj1->print_name[i] != '\0' && obj2->print_name[i] != '\0') {
	    if (obj1->print_name[i] != obj2->print_name[i])
		return false;
	    ++i;
	}
	return obj1->print_name[i] == obj2->print_name[i];
    }

    if (b_pair_pred(obj1))
	return b_equal_pred(car(obj1), car(obj2))
	    && b_equal_pred(cdr(obj1), cdr(obj2));

    if (obj1->type == TYPE_UNIQUE || b_function_pred(obj1))
	// We already know they're not the same object.
	return false;

    FOUND_BUG;
    exit(1);  // Avoid gcc warning about missing return.
}


// b_lt
// Builtin Lisp function <.
LispObject * b_lt(LispObject * obj1, LispObject * obj2) {
    if (!typecheck(obj1, LISP_INT_PRED_SYM)
	|| !typecheck(obj2, LISP_INT_PRED_SYM))
	return NULL;
    return (obj1->value < obj2->value ? LISP_T : LISP_EMPTY);
}
