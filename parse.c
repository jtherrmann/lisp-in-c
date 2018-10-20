// parse.c
// Source for parse functions.


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"
#include "parse.h"
#include "stack.h"


// TODO: parse negative ints
// TODO: ignore comments (; and anything after)


// ============================================================================
// Private function prototypes
// ============================================================================

LispObject * parseint();

LispObject * parsesym();

LispObject * parselist();

int power(int b, int n);

bool is_digit(char ch);


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
    if (is_digit(input[input_index]))
	return parseint();  // parseint fulfills parse's post.

    if (input[input_index] == '(') {
	// Skip the '(' to start at the first element of the list.
	input_index += 1;
	skipspace();  // Meet parselist's pre.
	return parselist();  // parselist fulfills parse's post.
    }

    // TODO: support ' (syntax for quote) and "" (syntax for str)
    if (input[input_index] != ')'
	&& input[input_index] != '\''
	&& input[input_index] != '"'
	&& input[input_index] != '.'
	&& input[input_index] != ':') {

	int start = input_index;
	LispObject * sym = parsesym();  // parsesym fulfills parse's post.

	if (sym == NULL)
	    return NULL;

	if (b_equal(sym, LISP_T_SYM))
	    return LISP_T;

	if (b_equal(sym, LISP_F_SYM))
	    return LISP_F;

	if (sym->print_name[0] == '#') {
	    input_index = start;
	    show_input_char();
	    printf("%s'#' unrecognized in this context\n", PARSE_ERR);
	    return NULL;
	}

	return sym;
    }

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
    for (int i = 0; i < input_index; ++i)
	printf(" ");
    printf("^\n");
}


// ============================================================================
// Private functions
// ============================================================================

// TODO: handle overflow; also, make ints as large as possible (e.g. signed
// longs or whatever)
//
// parseint
// Convert part of the input str to a Lisp int.
//
// Pre:
// - is_digit(input[input_index])
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
//
// On error:
// - Return NULL.
LispObject * parseint() {

    // Go to the end of the substr that represents the int.
    int begin = input_index;
    while (input[input_index] != '('
	   && input[input_index] != ')'
	   && input[input_index] != ' '
	   && input[input_index] != INPUT_END) {
    	if (!is_digit(input[input_index])) {
	    show_input_char();
	    printf("%snumber contains non-numeral '%c'\n",
		   PARSE_ERR, input[input_index]);
	    return NULL;
	}
    	++input_index;
    }
    int end = input_index;
    --input_index;

    // Go back through the substr, adding up the values of the digits to get
    // the overall value of the int.
    int total = 0;
    int place = 0;
    int digit;
    while (input_index >= begin) {
	digit = input[input_index] - 0x30;
	total += digit * power(10, place);
	++place;
	--input_index;
    }

    // Fulfill post.
    input_index = end;
    skipspace();

    return get_int(total);
}


// TODO: only allow sym to start with a-z, A-Z, or -, and only allow chars
// after the first char to be in that same set of chars or 0-9
//
// parsesym
// Convert part of the input str to a Lisp symbol.
//
// Pre:
// - input[input_index] is not a space, (, ), ', ", ., or :
// - !is_digit(input[input_index])
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
//
// On error:
// - Return NULL.
LispObject * parsesym() {

    // Go to the end of the substr that represents the symbol.
    int begin = input_index;
    while (input[input_index] != '('
	   && input[input_index] != ')'
	   && input[input_index] != ' '
	   && input[input_index] != INPUT_END) {
	if (input[input_index] == '\'' || input[input_index] == '"') {
	    show_input_char();
	    printf("%ssymbol contains invalid char '%c'\n",
		   PARSE_ERR, input[input_index]);
	    return NULL;
	}
	++input_index;
    }
    int end = input_index;
    skipspace();  // Fulfill post.
    return get_sym_by_substr(input, begin, end);

    // TODO: a LispObject of type LISP_SYM stores its print name and the length
    // of its print name; here, check if the substr denoted by begin
    // (inclusive) and end (exclusive) is equal to the print name of any symbol
    // in the symbol interns list (by first comparing lens and then comparing
    // chars if lens are equal) and if it is, return the interned symbol; if
    // it's not, create a new symbol, add it onto the front of the interns
    // list, and return it
    // - in lisp_obj, if the type is LISP_SYM, add the obj to the symbol
    //   interns list rather than the weakrefs list; then sweep the interns
    //   list for unmarked symbols during GC
    // - alternatively could store interns in a hash table to find out more
    //   quickly if one already exists; then sweep each list in the hash
    //   table during GC
    //   - K&R C p. 143
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
	return LISP_NIL;
    }

    if (input[input_index] == INPUT_END) {
	show_input_char();
	printf("%sincomplete list\n", PARSE_ERR);
	return NULL;
    }

    // Invariant: pre still true.

    LispObject * car = parse(input);  // parselist's pre meets parse's pre.

    if (car == NULL)
	return NULL;

    // Protect car from GC that could be triggered by parselist (when it calls
    // parse, above).
    push(car);

    LispObject * cdr = parselist(input);

    pop();

    if (cdr == NULL)
	return NULL;

    LispObject * cons = b_cons(car, cdr);
    return cons;
}


// power
// Return b to the power of n.
//
// Pre:
// - n >= 0
int power(int b, int n) {
    if (n == 0)
	return 1;

    int total = b;
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
