// parse.c
// Source for parse functions.


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "obj.h"
#include "tests.h"


// TODO: parse negative ints


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
	&& input[input_index] != '"')
	return parsesym();  // parsesym fulfills parse's post.
    
    printf("PARSE ERROR: unrecognized char\n");
    exit(1);
}


// skipspace
void skipspace() {
    while (input[input_index] == ' ')
	++input_index;
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
LispObject * parseint() {

    // Go to the end of the substr that represents the int.
    int start = input_index;
    while (input[input_index] != '('
	   && input[input_index] != ')'
	   && input[input_index] != ' '
	   && input[input_index] != INPUT_END) {
    	if (!is_digit(input[input_index])) {
    	    printf("PARSE ERROR: non-digit char in number\n");
	    exit(1);
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
    while (input_index >= start) {
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


// parsesym
// Convert part of the input str to a Lisp symbol.
//
// Pre:
// - input[input_index] is not a space, (, ), ', or "
// - !is_digit(input[input_index])
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
LispObject * parsesym() {

    // Go to the end of the substr that represents the symbol, in order to
    // validate it and get its length.
    int start = input_index;
    while (input[input_index] != '('
	   && input[input_index] != ')'
	   && input[input_index] != ' '
	   && input[input_index] != INPUT_END) {
	if (input[input_index] == '\'' || input[input_index] == '"') {
	    printf("PARSE ERROR: invalid char in symbol\n");
	    exit(1);
	}
	++input_index;
    }
    int end = input_index;
    input_index = start;
    // TODO: may change how start, end, and input_index are used here

    // TODO: a LispObject of type LISP_SYM stores its print name and the length
    // of its print name; here, check if the substr denoted by start
    // (inclusive) and end (exclusive) is equal to the print name of any symbol
    // in the symbol interns list (by first comparing lens and then comparing
    // chars if lens are equal) and if it is, return the interned symbol; if
    // it's not, create a new symbol, add it onto the front of the interns
    // list, and return it
    // - in lisp_obj, if the type is LISP_SYM, add the obj to the symbol
    //   interns list rather than the weakrefs list; then sweep the interns
    //   list for unmarked symbols during GC

    printf("PARSE ERROR: parsesym not implemented\n");
    exit(1);
}


// parselist
// Convert part of the input str to a Lisp list.
//
// Pre:
// - input[input_index] is not a space.
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
LispObject * parselist() {
    if (input[input_index] == ')') {
	// Fulfill post.
	++input_index;
	skipspace();

	// Return the empty list object.
	return LISP_NIL;
    }

    if (input[input_index] == INPUT_END) {
	printf("PARSE ERROR: incomplete list\n");
	exit(1);
    }

    // Invariant: pre still true.

    LispObject * car = parse(input);  // parselist's pre meets parse's pre.
    LispObject * cdr = parselist(input);
    return b_cons(car, cdr);
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


// ============================================================================
// Tests
// ============================================================================

void test_power() {
    assert(power(10, 0) == 1);
    assert(power(10, 1) == 10);
    assert(power(10, 2) == 100);
    assert(power(10, 3) == 1000);

    assert(power(-10, 0) == 1);
    assert(power(-10, 1) == -10);
    assert(power(-10, 2) == 100);
    assert(power(-10, 3) == -1000);

    assert(power(-15, 3) == -3375);
    assert(power(-12, 4) == 20736);
    assert(power(-6, 6) == 46656);
    assert(power(-4, 7) == -16384);

    assert(power(2, 12) == 4096);
    assert(power(3, 3) == 27);
    assert(power(5, 3) == 125);
    assert(power(6, 5) == 7776);
    assert(power(12, 4) == 20736);
    assert(power(15, 3) == 3375);
}


// TODO: how to append the value of INPUT_END, instead of '\n', to the test
// input strs?
void test_parse() {
    char input_int[] = "123\n";
    strcpy(input, input_int);
    input_index = 0;
    assert(objs_equal(parse(), get_int(123)));

    char input_nil[] = "()\n";
    strcpy(input, input_nil);
    input_index = 0;
    assert(objs_equal(parse(), LISP_NIL));


    // ------------------------------------------------------------------------
    // input_list1

    char input_list1[] = "(1 2 3)\n";
    strcpy(input, input_list1);
    input_index = 0;

    // (cons 1 (cons 2 (cons 3 NIL)))
    LispObject * list1 =
	b_cons(get_int(1),
		  b_cons(get_int(2),
			    b_cons(get_int(3), LISP_NIL)));

    assert(objs_equal(parse(), list1));


    // ------------------------------------------------------------------------
    // input_list2

    char input_list2[] = "(1 2 3 (20 30 ()) 500)\n";
    strcpy(input, input_list2);
    input_index = 0;

    // (cons 1 (cons 2 (cons 3 (cons (cons 20 (cons 30 (cons NIL NIL))) (cons 500 NIL)))))
    LispObject * list2 =
	b_cons(get_int(1),
		  b_cons(get_int(2),
			    b_cons(get_int(3),
				      b_cons(b_cons(get_int(20),
							  b_cons(get_int(30),
								    b_cons(LISP_NIL, LISP_NIL))),
						b_cons(get_int(500), LISP_NIL)))));

    assert(objs_equal(parse(), list2));
}
