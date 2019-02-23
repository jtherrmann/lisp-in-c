// error.c
// Source for error handling utilities.


#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "env.h"
#include "print.h"


// TODO: this function doesn't belong here because it's part of the Lisp error
// handling system
bool typecheck(LispObject * obj, LispObject * pred_sym) {
    ASSERT(b_symbol_pred(pred_sym));

    LispObject * pred_def = get_def(pred_sym);
    ASSERT(pred_def != NULL);
    ASSERT(pred_def->type == TYPE_BOOL_BUILTIN_1);

    bool result = pred_def->b_bool_func_1(obj);

    if (!result) {
	printf("Type error: (");
	print_obj(pred_sym);
	printf(" ");
	print_obj(obj);
	printf(")\n\n");
	return false;
    }
    return true;
}
