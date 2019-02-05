// print.c
// Source for print functions.


#include <stdio.h>

#include "error.h"
#include "print.h"


// ============================================================================
// Private function prototypes
// ============================================================================

void print_pair(LispObject * obj);


// ============================================================================
// Public functions
// ============================================================================

// print_obj
// Print a Lisp object.
void print_obj(LispObject * obj) {
    if (b_null_pred(obj))
	printf("()");

    else if (b_int_pred(obj))
	printf("%ld", obj->value);

    else if (b_symbol_pred(obj))
	printf("%s", obj->print_name);

    else if (b_pair_pred(obj))
	print_pair(obj);

    else if (obj->type == TYPE_LAMBDA) {
	printf("#<function>[");
	print_obj(obj->env_list);
	printf("]");
	print_obj(obj->args);
	printf("->");
	print_obj(obj->body);
    }

    else if (is_builtin(obj)) {
	printf("#<builtin function: ");
	print_obj(obj->builtin_name);
	printf(">");
    }

    else
	FOUND_BUG;
}


// ============================================================================
// Private functions
// ============================================================================

// print_pair
// Print up to the first 100 car values in a chain of Lisp pairs.
void print_pair(LispObject * obj) {
    ASSERT(b_pair_pred(obj));

    printf("(");

    long i = 0;
    while (true) {
	print_obj(car(obj));
	obj = cdr(obj);

	if (!b_pair_pred(obj))
	    break;

	printf(" ");

	++i;
	if (i == 100) {
	    printf("...");
	    return;
	}
    }

    if (!b_null_pred(obj)) {
	printf(" . ");
	print_obj(obj);
    }

    printf(")");
}
