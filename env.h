// env.h
// Header for the Lisp environment.


#ifndef ENV_H
#define ENV_H


#include "obj.h"


// ============================================================================
// env
// ============================================================================

#define HASHSIZE 101

struct binding {
    LispObject * name;
    LispObject * def;
    struct binding * next;
};

struct binding * env[HASHSIZE];


// ============================================================================
// Public functions
// ============================================================================

void bind(LispObject * sym, LispObject * def);
LispObject * get_def(LispObject * name);
void print_env();


#endif
