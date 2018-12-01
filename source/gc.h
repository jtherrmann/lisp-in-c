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

bool gc_output;


// ============================================================================
// Public functions
// ============================================================================

void collect_garbage();


#endif
