// parse.h
// Header for parse functions.


#ifndef PARSE_H
#define PARSE_H


#include "obj.h"


// ============================================================================
// Macros
// ============================================================================

#define INPUT_LEN 2048

#define INPUT_END '\n'

#define PARSE_ERR "Parse error: "


// ============================================================================
// Global variables
// ============================================================================

char input[INPUT_LEN];

int input_index;


// ============================================================================
// Public functions
// ============================================================================

LispObject * parse();

void skipspace();

void show_input_char();


#endif
