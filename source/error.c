// error.c
// Source for error handling utilities.


#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "env.h"
#include "print.h"


bool typecheck(LispObject * obj, LispObject * pred_sym) {
    if (!b_symbol_pred(pred_sym))
	FOUND_BUG;

    LispObject * pred_def = get_def(pred_sym);
    if (pred_def == NULL)
	FOUND_BUG;

    if (pred_def->type != TYPE_BOOL_BUILTIN_1)
	FOUND_BUG;

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


void found_bug(const char * file, int line, const char * func) {
    // TODO: instructions for reporting
    printf("\n%s, line %d, in %s:\nIt looks like you have found a bug!\n\n",
	   file, line, func);
    exit(1);
}
