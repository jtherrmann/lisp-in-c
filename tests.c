// tests.c
// Source for tests.


#include <assert.h>
#include <stdio.h>

#include "obj.h"
#include "parse.h"
#include "tests.h"


// Main internal test function.
void test_tests();


// ============================================================================
// Public functions
// ============================================================================

void run_tests() {
    printf("Running tests.\n");

    test_tests();
    test_parse();

    printf("All tests pass.\n\n");
}


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
// Tests
// ============================================================================

// ----------------------------------------------------------------------------
// Prototypes
// ----------------------------------------------------------------------------

void test_objs_equal();


// ----------------------------------------------------------------------------
// Definitions
// ----------------------------------------------------------------------------

// Main internal test function.
void test_tests() {
    test_objs_equal();
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
