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
// - b_cons_pred(obj)
void print_list(LispObject * obj);


// ============================================================================
// Public functions
// ============================================================================

void print_obj(LispObject * obj) {
    if (b_null_pred(obj))
	printf("()");

    else if (obj == LISP_T)
	print_obj(LISP_T_SYM);

    else if (obj == LISP_F)
	print_obj(LISP_F_SYM);

    else if (b_number_pred(obj))
	printf("%d", obj->value);

    else if (b_symbol_pred(obj))
	printf("%s", obj->print_name);

    else if (b_cons_pred(obj))
	// TODO: print lists properly but maybe leave this version in as a
	// debug option
	/* printf("(cons "); */
	/* print_obj(b_car(obj)); */
	/* printf(" "); */
	/* print_obj(b_cdr(obj)); */
	/* printf(")"); */
	print_list(obj);

    else if (b_func_pred(obj)) {
	printf("#<function>");
	print_obj(obj->args);
	printf("->");
	print_obj(obj->body);
    }

    else if (b_builtin_pred(obj)) {
	printf("#<builtin function: ");
	print_obj(obj->builtin_name);
	printf(">");
    }

    else {
	printf("PRINT ERROR: unrecognized type\n");
	exit(1);
    }
}


// ============================================================================
// Private functions
// ============================================================================

// print_list
// Print up to the first 100 items of a Lisp list.
void print_list(LispObject * obj) {
    printf("(");

    int i = 0;
    while (true) {
	print_obj(b_car(obj));
	obj = b_cdr(obj);

	if (!b_cons_pred(obj))
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
