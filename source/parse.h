// parse.h
// Header for parse functions.


#ifndef PARSE_H
#define PARSE_H


#include "obj.h"


// ============================================================================
// Macros
// ============================================================================

#define INPUT_END '\0'

#define PARSE_ERR "Parse error: "


// ============================================================================
// Global variables
// ============================================================================

char * input;

long input_index;


// ============================================================================
// Public functions
// ============================================================================

LispObject * parse();

void skipspace();

void show_input_char();


#endif
