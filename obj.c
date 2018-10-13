// obj.c
// Source for functions related to Lisp objects.


#include <assert.h>
#include <stdio.h>

#include "obj.h"
#include "gc.h"


// ============================================================================
// Private functions
// ============================================================================
//
// Functions not declared in header.

LispObject * get_obj(LispType type) {
    LispObject * obj = malloc(sizeof(LispObject));
    // TODO: check for malloc error code?
    obj->type = type;
    obj->marked = false;
    obj->weakref = weakrefs_head;
    weakrefs_head = obj;
    ++weakrefs_count;
    return obj;
}

LispObject * get_nil() {
    LispObject * obj = get_obj(LISP_NILTYPE);
    obj->car = obj;
    obj->cdr = obj;
    return obj;
}


// ============================================================================
// LispObject
// ============================================================================

// TODO: these objs need to be marked during GC--or maybe just don't add them
// to weakrefs list so they can't be swept; easy way to do this would be to set
// weakrefs_head to NULL in main AFTER calling this func
//
// or maybe just don't even bother with any of these except for LISP_NIL
LispObject * make_initial_objs() {
    LISP_NIL = get_nil();

    char quote[] = "quote";
    LISP_QUOTE = get_sym(quote);

    char def[] = "def";
    LISP_DEF = get_sym(def);

    char lambda[] = "lambda";
    LISP_LAMBDA = get_sym(lambda);
}


// ----------------------------------------------------------------------------
// Internal constructors
// ----------------------------------------------------------------------------

LispObject * get_int(int value) {
    LispObject * obj = get_obj(LISP_INT);
    obj->value = value;
}


LispObject * get_sym(char * str) {
    int len = 0;
    while (str[len] != '\0')
	++len;

    LispObject * obj = get_obj(LISP_SYM);
    obj->print_name = malloc((len + 1) * sizeof(char));

    for (int i = 0; i < len; ++i)
	obj->print_name[i] = str[i];
    obj->print_name[len] = '\0';
    
    return obj;
}


LispObject * get_sym_by_substr(char * str, int begin, int end) {
    int len = end - begin;

    LispObject * obj = get_obj(LISP_SYM);
    obj->print_name = malloc((len + 1) * sizeof(char));

    for (int i = 0; i < len; ++i)
	obj->print_name[i] = str[begin + i];
    obj->print_name[len] = '\0';

    return obj;
}


LispObject * get_func(LispObject * args, LispObject * body) {
    // TODO: proper errors
    assert(b_listp(args));
    assert(b_listp(body));

    LispObject * obj = get_obj(LISP_FUNC);
    obj->args = args;
    obj->body = body;
}


// ----------------------------------------------------------------------------
// cons, car, and cdr
// ----------------------------------------------------------------------------

LispObject * b_cons(LispObject * car, LispObject * cdr) {
    LispObject * obj = get_obj(LISP_CONS);
    obj->car = car;
    obj->cdr = cdr;
    return obj;
}


LispObject * b_car(LispObject * obj) {
    // TODO: proper typecheck
    assert(b_listp(obj));
    return obj->car;
}


LispObject * b_cdr(LispObject * obj) {
    // TODO: proper typecheck
    assert(b_listp(obj));
    return obj->cdr;
}


// ============================================================================
// Type predicates
// ============================================================================

bool b_null(LispObject * obj) {
    return obj == LISP_NIL;
}


bool b_numberp(LispObject * obj) {
    return obj->type == LISP_INT;
}


bool b_symbolp(LispObject * obj) {
    return obj->type == LISP_SYM;
}


bool b_consp(LispObject * obj) {
    return obj->type == LISP_CONS;
}


bool b_listp(LispObject * obj) {
    return b_consp(obj) || b_null(obj);
}


bool b_funcp(LispObject * obj) {
    return obj->type == LISP_FUNC;
}


// ============================================================================
// Comparison functions
// ============================================================================

bool b_equal(LispObject * obj1, LispObject * obj2) {
    if (obj1 == obj2)
	return true;

    if (obj1->type != obj2->type)
	return false;

    if (b_numberp(obj1))
	return obj1->value == obj2->value;

    if (b_symbolp(obj1)) {
	// TODO: tests
	// TODO: once string interning implemented, just compare str ptrs
	int i = 0;
	while (obj1->print_name[i] != '\0' && obj2->print_name[i] != '\0') {
	    if (obj1->print_name[i] != obj2->print_name[i])
		return false;
	    ++i;
	}
	return obj1->print_name[i] == obj2->print_name[i];
    }

    if (b_consp(obj1))
	return b_equal(b_car(obj1), b_car(obj2))
	    && b_equal(b_cdr(obj1), b_cdr(obj2));

    printf("COMPARISON ERROR: unrecognized type\n");
    exit(1);
}


// ============================================================================
// Tests
// ============================================================================

// ----------------------------------------------------------------------------
// Prototypes
// ----------------------------------------------------------------------------

void test_b_equal();


// ----------------------------------------------------------------------------
// Definitions
// ----------------------------------------------------------------------------

// Public test function.
int test_obj() {
    int total = 0;

    test_b_equal();
    ++total;

    return total;
}


void test_b_equal() {
    LispObject * obj1;
    LispObject * obj2;

    obj1 = get_int(1);
    obj2 = LISP_NIL;
    assert(!b_equal(obj1, obj2));

    obj1 = LISP_NIL;
    obj2 = LISP_NIL;
    assert(b_equal(obj1, obj2));

    obj1 = get_int(3);
    obj2 = get_int(5);
    assert(!b_equal(obj1, obj2));

    obj1 = get_int(3);
    obj2 = get_int(3);
    assert(b_equal(obj1, obj2));

    obj1 = b_cons(get_int(1), LISP_NIL);
    obj2 = b_cons(get_int(2), LISP_NIL);
    assert(!b_equal(obj1, obj2));

    obj1 = b_cons(get_int(1), LISP_NIL);
    obj2 = b_cons(get_int(1), LISP_NIL);
    assert(b_equal(obj1, obj2));

    obj1 = b_cons(get_int(5), get_int(10));
    obj2 = b_cons(get_int(3), get_int(10));
    assert(!b_equal(obj1, obj2));

    obj1 = b_cons(get_int(5), get_int(10));
    obj2 = b_cons(get_int(5), get_int(10));
    assert(b_equal(obj1, obj2));

    obj1 = b_cons(get_int(1),
		     b_cons(get_int(2),
			       b_cons(get_int(3), get_int(3))));
    obj2 = b_cons(get_int(1),
		     b_cons(get_int(2),
			       b_cons(get_int(3), LISP_NIL)));
    assert(!b_equal(obj1, obj2));

    obj1 = b_cons(get_int(1),
		     b_cons(get_int(2),
			       b_cons(get_int(3), LISP_NIL)));
    obj2 = b_cons(get_int(1),
		     b_cons(get_int(2),
			       b_cons(get_int(3), LISP_NIL)));
    assert(b_equal(obj1, obj2));

    obj2 = LISP_NIL;
    assert(!b_equal(obj1, obj2));

    obj2 = obj1;
    assert(b_equal(obj1, obj2));
}
