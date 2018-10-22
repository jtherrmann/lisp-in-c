// binops.c
// Source for builtin binary operators.


#include "binops.h"
#include "misc.h"


// ============================================================================
// Comparison functions
// ============================================================================

bool b_equal(LispObject * obj1, LispObject * obj2) {
    if (obj1 == obj2)
	return true;

    if (obj1->type != obj2->type)
	return false;

    if (obj1->type == TYPE_UNIQUE)
	return obj1 == obj2;

    if (b_number_pred(obj1))
	return obj1->value == obj2->value;

    if (b_symbol_pred(obj1)) {
	// TODO: once string interning implemented, just compare str ptrs (or
	// intern entire symbols, in which case remove this if block because
	// the first if (obj1 == obj2) will execute)
	int i = 0;
	while (obj1->print_name[i] != '\0' && obj2->print_name[i] != '\0') {
	    if (obj1->print_name[i] != obj2->print_name[i])
		return false;
	    ++i;
	}
	return obj1->print_name[i] == obj2->print_name[i];
    }

    if (b_cons_pred(obj1))
	return b_equal(b_car(obj1), b_car(obj2))
	    && b_equal(b_cdr(obj1), b_cdr(obj2));

    FOUND_BUG;
    exit(1);  // Avoid gcc warning about missing return.
}
