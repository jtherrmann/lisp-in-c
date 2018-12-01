// parse.c
// Source for parse functions.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "builtins.h"
#include "obj.h"
#include "stack.h"


// ============================================================================
// Private function prototypes
// ============================================================================

LispObject * parseint();

LispObject * parsesym();

LispObject * parsebool();

LispObject * parselist();

long power(long b, long n);

bool is_digit(char ch);

bool is_sym_char(char ch);

bool is_sym_start_char(char ch);


// ============================================================================
// Public functions
// ============================================================================

// parse
// Convert part of the input str to a Lisp object.
//
// Pre:
// - input[input_index] is a non-space char.
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
//
// On error:
// - Return NULL.
LispObject * parse() {
    if (is_digit(input[input_index])
	|| (input[input_index] == '-' && is_digit(input[input_index + 1])))
	return parseint();  // parseint fulfills parse's post.

    if (input[input_index] == '(') {
	// Skip the '(' to start at the first element of the list.
	input_index += 1;
	skipspace();  // Meet parselist's pre.
	return parselist();  // parselist fulfills parse's post.
    }

    if (is_sym_start_char(input[input_index]))
	return parsesym();  // parsesym fulfills parse's post.

    if (input[input_index] == '#')
	return parsebool();  // parsebool fulfills parse's post.

    show_input_char();
    printf("%s'%c' unrecognized in this context\n",
	   PARSE_ERR, input[input_index]);
    return NULL;
}


// skipspace
void skipspace() {
    while (input[input_index] == ' ')
	++input_index;
}


// show_input_char
// Print a '^' char below the current input char.
//
// Pre:
// - input[input_index] is the current input char.
// - The REPL prompt is two chars wide.
void show_input_char() {
    printf("  ");
    for (long i = 0; i < input_index; ++i)
	printf(" ");
    printf("^\n");
}


// ============================================================================
// Private functions
// ============================================================================

// TODO: handle overflow
//
// parseint
// Convert part of the input str to a Lisp int.
//
// Pre:
// - is_digit(input[input_index])
//   || (input[input_index] == '-' && is_digit(input[input_index + 1]))
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
//
// On error:
// - Return NULL.
LispObject * parseint() {

    bool positive = true;
    if (input[input_index] == '-') {
	positive = false;
	++input_index;
    }

    // Go to the end of the substr that represents the int.
    long begin = input_index;
    while (input[input_index] != '('
	   && input[input_index] != ')'
	   && input[input_index] != ' '
	   && input[input_index] != INPUT_END
	   && input[input_index] != ';') {
    	if (!is_digit(input[input_index])) {
	    show_input_char();
	    printf("%snumber contains non-numeral '%c'\n",
		   PARSE_ERR, input[input_index]);
	    return NULL;
	}
    	++input_index;
    }
    long end = input_index;
    --input_index;

    // Go back through the substr, adding up the values of the digits to get
    // the overall value of the int.
    long total = 0;
    long place = 0;
    long digit;
    while (input_index >= begin) {
	digit = input[input_index] - 0x30;
	total += digit * power(10, place);
	++place;
	--input_index;
    }

    // Fulfill post.
    input_index = end;
    skipspace();

    return get_int(positive ? total : -total);
}


// parsesym
// Convert part of the input str to a Lisp symbol.
//
// Pre:
// - is_sym_start_char(input[input_index])
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
//
// On error:
// - Return NULL.
LispObject * parsesym() {

    // Go to the end of the substr that represents the symbol.
    long begin = input_index;
    while (input[input_index] != '('
	   && input[input_index] != ')'
	   && input[input_index] != ' '
	   && input[input_index] != INPUT_END
	   && input[input_index] != ';') {
	if (!is_sym_char(input[input_index])) {
	    show_input_char();
	    printf("%ssymbol contains invalid char '%c'\n",
		   PARSE_ERR, input[input_index]);
	    return NULL;
	}
	++input_index;
    }
    long end = input_index;
    skipspace();  // Fulfill post.
    return get_sym_by_substr(input, begin, end);
}


// parsebool
// Convert part of the input str to a Lisp bool.
//
// Pre:
// - input[input_index] is '#'.
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
//
// On error:
// - Return NULL.
LispObject * parsebool() {
    ++input_index;
    char boolch = input[input_index];

    if (boolch != 't' && boolch != 'f') {
	show_input_char();
	printf("%sexpected 't' or 'f' but got '%c'\n", PARSE_ERR, boolch);
	return NULL;
    }

    ++input_index;
    if (input[input_index] != '('
	&& input[input_index] != ')'
	&& input[input_index] != ' '
	&& input[input_index] != INPUT_END
	&& input[input_index] != ';') {
	show_input_char();
	printf("%sbool followed by invalid char '%c'\n",
	       PARSE_ERR, input[input_index]);
	return NULL;
    }

    skipspace();  // Fulfill post.
    return (boolch == 't' ? LISP_T : LISP_F);
}


// parselist
// Convert part of the input str to a Lisp list.
//
// Pre:
// - input[input_index] is not a space.
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
//
// On error:
// - Return NULL.
LispObject * parselist() {
    if (input[input_index] == ')') {
	// Fulfill post.
	++input_index;
	skipspace();

	// Return the empty list object.
	return LISP_EMPTY;
    }

    if (input[input_index] == INPUT_END || input[input_index] == ';') {
	show_input_char();
	printf("%sincomplete list\n", PARSE_ERR);
	return NULL;
    }

    // Invariant: pre still true.

    LispObject * car = parse();  // parselist's pre meets parse's pre.

    if (car == NULL)
	return NULL;

    // Protect car from GC that could be triggered by parselist (when it calls
    // parse, above).
    push(car);

    LispObject * cdr = parselist();

    pop();

    if (cdr == NULL)
	return NULL;

    LispObject * pair = b_cons(car, cdr);
    return pair;
}


// power
// Return b to the power of n.
//
// Pre:
// - n >= 0
long power(long b, long n) {
    if (n == 0)
	return 1;

    long total = b;
    while (n > 1) {
	total *= b;
	--n;
    }
    return total;
}


// is_digit
// Return whether the char is in the range '0'-'9'.
bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}


// is_sym_char
// Return whether a symbol can contain the char.
bool is_sym_char(char ch) {
    return is_sym_start_char(ch) || (ch >= '0' && ch <= '9');
}


// is_sym_start_char
// Return whether a symbol can start with the char.
bool is_sym_start_char(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '?'
	|| ch == '+' || ch == '-' || ch == '/' || ch == '*' || ch == '<'
	|| ch == '>' || ch == '=';
}
