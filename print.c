// print.c
// Source for print functions.


#include <stdio.h>

#include "print.h"


// ============================================================================
// Private function prototypes
// ============================================================================

// print_list
// Print a non-empty Lisp list.
//
// Pre:
// - b_consp(obj)
void print_list(LispObject * obj);


// ============================================================================
// Public functions
// ============================================================================

void print_obj(LispObject * obj) {
    if (b_null(obj))
	printf("NIL");

    else if (b_numberp(obj))
	printf("%d", obj->value);

    else if (b_symbolp(obj))
	printf("%s", obj->print_name);

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


// ============================================================================
// Private functions
// ============================================================================

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
