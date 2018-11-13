// eval.h
// Header for eval functions.


#ifndef EVAL_H
#define EVAL_H


#include "obj.h"


LispObject * b_eval(LispObject * expr, LispObject * env, bool toplevel);


#endif
