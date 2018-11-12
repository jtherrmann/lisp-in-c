// binops.h
// Header for builtin binary operators.


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
// Boolean logic
// ============================================================================

LispObject * b_and(LispObject * obj1, LispObject * obj2);

LispObject * b_or(LispObject * obj1, LispObject * obj2);

LispObject * b_not(LispObject * obj);


// ============================================================================
// Comparison functions
// ============================================================================

bool b_equal(LispObject * obj1, LispObject * obj2);


#endif
