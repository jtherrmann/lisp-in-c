// error.h
// Header for error handling utilities.


#ifndef ERROR_H
#define ERROR_H


#include <stdbool.h>

#include "obj.h"


#define ASSERT(EXPR) if(!(EXPR)) { PRINT_LOCATION; printf("Assertion failed:\n%s\n\n", #EXPR); exit(1); }

#define FOUND_BUG PRINT_LOCATION; printf("It looks like you have found a bug!\n\n"); exit(1);

#define PRINT_LOCATION printf("\n%s, line %d, in %s:\n", __FILE__, __LINE__, __func__);

bool typecheck(LispObject * obj, LispObject * pred_sym);


#endif
