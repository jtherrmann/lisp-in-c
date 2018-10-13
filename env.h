// env.h
// Header for the Lisp environment.


#ifndef ENV_H
#define ENV_H


#include "obj.h"


void bind(LispObject * sym, LispObject * def);
LispObject * get_def(LispObject * name);
void print_env();


#endif
