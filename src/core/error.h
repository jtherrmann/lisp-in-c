// error.h
// Header for error handling utilities.


#ifndef MISC_H
#define MISC_H


#include <stdbool.h>

#include "obj.h"


#define ASSERT(b) if (!(b)) FOUND_BUG;

#define FOUND_BUG found_bug(__FILE__, __LINE__, __func__);


bool typecheck(LispObject * obj, LispObject * pred_sym);

void found_bug(const char * file, long line, const char * func);


#endif
