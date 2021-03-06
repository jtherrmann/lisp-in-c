// gc.h
// Header for garbage collector.


#ifndef GC_H
#define GC_H


#include "obj.h"


// ============================================================================
// Global variables
// ============================================================================

LispObject * weakrefs_head;

unsigned long weakrefs_count;


// ============================================================================
// Public functions
// ============================================================================

void collect_garbage();

LispObject * b_print_weakrefs();


#endif
