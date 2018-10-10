// gc.h
// Header for garbage collector.


#ifndef GC_H
#define GC_H


#include "builtins.h"


// ============================================================================
// Global variables
// ============================================================================

LispObject * weakrefs_head;


// ============================================================================
// Debugging
// ============================================================================

// TODO: remove these when they're no longer needed or make them available as
// special interpreter commands

void print_weakrefs();

void free_all();


#endif
