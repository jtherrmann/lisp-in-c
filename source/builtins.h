// builtins.h
// Header for miscellaneous builtin functions.


#ifndef BINOPS_H
#define BINOPS_H


#include <stdbool.h>

#include "obj.h"


// ============================================================================
// Arithmetic
// ============================================================================

LispObject * b_add(LispObject * obj1, LispObject * obj2);

LispObject * b_sub(LispObject * obj1, LispObject * obj2);

LispObject * b_mul(LispObject * obj1, LispObject * obj2);

LispObject * b_div(LispObject * obj1, LispObject * obj2);


// ============================================================================
// Comparison functions
// ============================================================================

bool b_equal_pred(LispObject * obj1, LispObject * obj2);

LispObject * b_lt(LispObject * obj1, LispObject * obj2);


#endif
