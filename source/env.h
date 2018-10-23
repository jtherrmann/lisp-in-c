// env.h
// Header for the global Lisp environment.


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
    bool constant;
};

struct binding * global_env[HASHSIZE];


// ============================================================================
// Public functions
// ============================================================================

bool bind(LispObject * sym, LispObject * def, bool constant);

LispObject * get_def(LispObject * name);


#endif
