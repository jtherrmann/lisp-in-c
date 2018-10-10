// TODO:
// - address TODO/FIXME throughout files
// - split into multiple files, *.c or *.h as appropriate
// - temp sources list:
//   - http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
//   - https://carld.github.io/2017/06/20/lisp-in-less-than-200-lines-of-c.html
// - add tests
// - remove unneeded #include lines
// - comment all func prototypes (name, summary, pre & post)

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtins.h"


// ============================================================================
// Macros
// ============================================================================

#define INPUT_LEN 2048
#define INPUT_END '\n'


// ============================================================================
// Global variables
// ============================================================================

char input[INPUT_LEN];
int input_index;


// ============================================================================
// Parse
// ============================================================================

LispObject * parseint();
LispObject * parselist();
LispObject * parsesym();
bool is_digit(char ch);
int power(int, int);
void skipspace();

// TODO: parse negative ints

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


// TODO: handle overflow; also, make ints as large as possible (e.g. signed
// longs or whatever)
//
// parseint
// Convert part of the input str to a Lisp int.
//
// Pre:
// - input[input_index] is a char in the range '0'-'9'.
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


bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
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


// parsesym
// Convert part of the input str to a Lisp symbol.
//
// Pre:
// - input[input_index] is a non-space char that is not (, ), ', ", or a digit
//   in the range 0-9.
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
// - input[input_index] is a non-space char.
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


void skipspace() {
    while (input[input_index] == ' ')
	++input_index;
}


// ============================================================================
// Print
// ============================================================================

void print_list(LispObject * obj);

void print_obj(LispObject * obj) {
    if (b_null(obj))
	printf("NIL");

    else if (b_numberp(obj))
	printf("%d", obj->value);

    else if (b_consp(obj))
	// TODO: print lists properly but maybe leave this version in as a
	// debug option
	/* printf("(cons "); */
	/* print_obj(b_car(obj)); */
	/* printf(" "); */
	/* print_obj(b_cdr(obj)); */
	/* printf(")"); */
	print_list(obj);

    else {
	printf("PRINT ERROR: unrecognized type\n");
	exit(1);
    }
}


// print_list
// Print a non-empty Lisp list.
//
// Pre:
// - b_consp(obj)
void print_list(LispObject * obj) {
    printf("(");
    while (true) {
	print_obj(b_car(obj));
	obj = b_cdr(obj);
	if (!b_consp(obj))
	    break;
	printf(" ");
    }
    if (!b_null(obj)) {
	printf(" . ");
	print_obj(obj);
    }
    printf(")");
}


// ============================================================================
// Tests
// ============================================================================

bool objs_equal(LispObject * obj1, LispObject * obj2);


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


void test_objs_equal() {
    LispObject * obj1;
    LispObject * obj2;

    obj1 = get_int(1);
    obj2 = LISP_NIL;
    assert(!objs_equal(obj1, obj2));

    obj1 = LISP_NIL;
    obj2 = LISP_NIL;
    assert(objs_equal(obj1, obj2));

    obj1 = get_int(3);
    obj2 = get_int(5);
    assert(!objs_equal(obj1, obj2));

    obj1 = get_int(3);
    obj2 = get_int(3);
    assert(objs_equal(obj1, obj2));

    obj1 = b_cons(get_int(1), LISP_NIL);
    obj2 = b_cons(get_int(2), LISP_NIL);
    assert(!objs_equal(obj1, obj2));

    obj1 = b_cons(get_int(1), LISP_NIL);
    obj2 = b_cons(get_int(1), LISP_NIL);
    assert(objs_equal(obj1, obj2));

    obj1 = b_cons(get_int(5), get_int(10));
    obj2 = b_cons(get_int(3), get_int(10));
    assert(!objs_equal(obj1, obj2));

    obj1 = b_cons(get_int(5), get_int(10));
    obj2 = b_cons(get_int(5), get_int(10));
    assert(objs_equal(obj1, obj2));

    obj1 = b_cons(get_int(1),
		     b_cons(get_int(2),
			       b_cons(get_int(3), get_int(3))));
    obj2 = b_cons(get_int(1),
		     b_cons(get_int(2),
			       b_cons(get_int(3), LISP_NIL)));
    assert(!objs_equal(obj1, obj2));

    obj1 = b_cons(get_int(1),
		     b_cons(get_int(2),
			       b_cons(get_int(3), LISP_NIL)));
    obj2 = b_cons(get_int(1),
		     b_cons(get_int(2),
			       b_cons(get_int(3), LISP_NIL)));
    assert(objs_equal(obj1, obj2));

    obj2 = LISP_NIL;
    assert(!objs_equal(obj1, obj2));

    obj2 = obj1;
    assert(objs_equal(obj1, obj2));
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


void run_tests() {
    printf("Running tests.\n");

    test_objs_equal();
    test_power();
    test_parse();

    printf("All tests pass.\n\n");
}


// ----------------------------------------------------------------------------
// Test utilities
// ----------------------------------------------------------------------------

bool objs_equal(LispObject * obj1, LispObject * obj2) {
    if (obj1 == obj2)
	return true;

    if (obj1->type != obj2->type)
	return false;

    if (b_numberp(obj1))
	return obj1->value == obj2->value;

    if (b_consp(obj1))
	return objs_equal(b_car(obj1), b_car(obj2))
	    && objs_equal(b_cdr(obj1), b_cdr(obj2));

    printf("COMPARISON ERROR: unrecognized type\n");
    exit(1);
}


// ============================================================================
// Main
// ============================================================================

int main() {

    /* LispObject * x = get_int(1); */
    /* print_obj(x); */
    /* printf("\n"); */
    /* printf("%d\n", x->value); */

    /* LispObject * y = get_int(2); */
    /* print_obj(y); */
    /* printf("\n"); */
    /* printf("%d\n", y->value); */

    /* LispObject * c = b_cons(x, y); */
    /* print_obj(c); */
    /* printf("\n"); */
    /* printf("%d\n", x); */
    /* printf("%d\n", b_car(c)); */
    /* printf("%d\n", b_car(c)->value); */
    /* printf("%d\n", b_cdr(c)->value); */

    /* LispObject * c2 = b_cons(x, c); */
    /* print_obj(c2); */
    /* printf("\n"); */

    /* LispObject * z = get_int(3); */
    /* LispObject * c3 = b_cons(x, b_cons(y, b_cons(z, LISP_NIL))); */
    /* print_obj(c3); */
    /* printf("\n"); */

    // creating c3 adds to weakrefs:
    // (cons 1 (cons 2 (cons 3 NIL))) -> (cons 2 (cons 3 NIL)) -> (cons 3 NIL)

    /* LispObject * head = weakrefs_head; */
    /* LispObject * next = weakrefs_head->weakref; */
    /* free(weakrefs_head); */
    /* weakrefs_head = next; */
    /* print_weakrefs(); */
    /* print_obj(head); */

    /* free_all(); */

    /* print_weakrefs(); */

    /* long i = 1; */
    /* while (true) { */
    /* 	lisp_obj(LISP_INT); */
    /* 	if (i % 100000000 == 0) { */
    /* 	    free_all(); // without this line we get memory leak */
    /* 	    i = 1; */
    /* 	} */
    /* 	++i; */
    /* } */


    /* LispObject * x = get_int(1); */
    /* LispObject * y = get_int(2); */
    /* LispObject * c = b_cons(x, b_cons(x, y)); */
    /* LispObject * c = b_cons(x, b_cons(b_cons(x, y), b_cons(x, LISP_NIL))); */
    /* print_obj(c); */
    /* printf("\n"); */

    LISP_NIL = get_nil();

    // TODO: add to tests
    assert(b_car(LISP_NIL) == LISP_NIL && b_cdr(LISP_NIL) == LISP_NIL);

    run_tests();

    printf("Welcome to Lisp!\n");
    printf("Exit with Ctrl-c\n\n");

    while (true) {
    	fputs("> ", stdout);
    	fgets(input, INPUT_LEN, stdin);

	input_index = 0;
	skipspace();  // Meet parse's pre.

	if (input[input_index] != INPUT_END) {
	    LispObject * obj = parse();

	    if (input[input_index] != INPUT_END) {
		printf("PARSE ERROR: multiple expressions\n");
		exit(1);
	    }

	    print_obj(obj);
	    printf("\n");
	}
    }

    return 0;
}
