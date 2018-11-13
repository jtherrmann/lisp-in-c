// binops.c
// Source for builtin binary operators.

#include <stdio.h>

#include "binops.h"
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

// b_equal_pred
// Builtin Lisp function equal?.
bool b_equal_pred(LispObject * obj1, LispObject * obj2) {
    if (obj1 == obj2)
	return true;

    if (obj1->type != obj2->type)
	return false;

    if (obj1->type == TYPE_UNIQUE)
	// We already know they're not the same object.
	return false;

    if (b_int_pred(obj1))
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

    if (b_pair_pred(obj1))
	return b_equal_pred(car(obj1), car(obj2))
	    && b_equal_pred(cdr(obj1), cdr(obj2));

    FOUND_BUG;
    exit(1);  // Avoid gcc warning about missing return.
}


// b_lt
// Builtin Lisp function <.
LispObject * b_lt(LispObject * obj1, LispObject * obj2) {
    if (!typecheck(obj1, LISP_INT_PRED_SYM)
	|| !typecheck(obj2, LISP_INT_PRED_SYM))
	return NULL;
    return (obj1->value < obj2->value ? LISP_T : LISP_F);
}


// b_lte
// Builtin Lisp function <=.
LispObject * b_lte(LispObject * obj1, LispObject * obj2) {
    if (!typecheck(obj1, LISP_INT_PRED_SYM)
	|| !typecheck(obj2, LISP_INT_PRED_SYM))
	return NULL;
    return (obj1->value <= obj2->value ? LISP_T : LISP_F);
}


// b_gt
// Builtin Lisp function >.
LispObject * b_gt(LispObject * obj1, LispObject * obj2) {
    if (!typecheck(obj1, LISP_INT_PRED_SYM)
	|| !typecheck(obj2, LISP_INT_PRED_SYM))
	return NULL;
    return (obj1->value > obj2->value ? LISP_T : LISP_F);
}


// b_gte
// Builtin Lisp function >=.
LispObject * b_gte(LispObject * obj1, LispObject * obj2) {
    if (!typecheck(obj1, LISP_INT_PRED_SYM)
	|| !typecheck(obj2, LISP_INT_PRED_SYM))
	return NULL;
    return (obj1->value >= obj2->value ? LISP_T : LISP_F);
}
