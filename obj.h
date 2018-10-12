// obj.h
// Header for Lisp objects and related functions.


#ifndef OBJ_H
#define OBJ_H


#include <stdbool.h>
#include <stdlib.h>


// ============================================================================
// typedefs
// ============================================================================

typedef struct LispObjectStruct LispObject;

// TODO: weird indentation
typedef enum {
	      LISP_INT,
	      LISP_SYM,
	      LISP_CONS,
	      LISP_NILTYPE
} LispType;


// ============================================================================
// Global variables
// ============================================================================

LispObject * LISP_NIL;
LispObject * LISP_QUOTE;


// ============================================================================
// LispObject
// ============================================================================

struct LispObjectStruct {
    LispType type;

    // TODO: temp notes:
    // - https://www.geeksforgeeks.org/g-fact-38-anonymous-union-and-structure/
    //   - struct/union without tag is anonymous (its members can be accessed
    //     directly, in the scope where it was defined)
    // - https://www.geeksforgeeks.org/union-c/
    //   - union is a struct whose size is determined by its largest member and
    //     whose members share the same memory location
    // - so below we declare an anonymous union, so its members can be accessed
    //   directly from the LispObject object, and the union has an int value
    //   and an anonymous struct, so we can directly access the int value AND
    //   both of the anonymous struct's members (car and cdr) all from the
    //   LispObject object; plus, the anonymous union's size is just large
    //   enough to hold its largest member (which must be the anonymous
    //   struct), and its members (the int value and the anonymous struct)
    //   share the same location in memory; so LispObjects that are treated as
    //   ints (i.e. their int value members are used, but not their car and cdr
    //   members) have some wasted space, but LispObjects that are treated as
    //   cons cells (their car and cdr members are used, but not their int
    //   value members) don't waste any space
    //   - so if we have a LispObject* called obj and we do obj->car, we are
    //     directly accessing obj's anonymous union member and then directly
    //     accessing the anonymous union's anonymous struct member and then
    //     accessing the anonymous struct's car member
    union {
	// LISP_INT
	int value;

	// LISP_SYM
	char * print_name;

	// LISP_CONS and LISP_NILTYPE
	struct {
	    LispObject * car;
	    LispObject * cdr;
	};
    };

    LispObject * weakref;

};


LispObject * make_initial_objs();


// ----------------------------------------------------------------------------
// Internal constructors
// ----------------------------------------------------------------------------

// get_int
// Construct a Lisp int.
LispObject * get_int(int value);

// get_sym
// Construct a Lisp symbol from str.
LispObject * get_sym(char * str);

// get_sym_by_substr
// Construct a Lisp symbol from a substr of str.
LispObject * get_sym_by_substr(char * str, int begin, int end);

// ----------------------------------------------------------------------------
// cons, car, and cdr
// ----------------------------------------------------------------------------

LispObject * b_cons(LispObject * car, LispObject * cdr);

LispObject * b_car(LispObject * obj);

LispObject * b_cdr(LispObject * obj);


// ============================================================================
// Type predicates
// ============================================================================

bool b_null(LispObject * obj);

bool b_numberp(LispObject * obj);

bool b_symbolp(LispObject * obj);

bool b_consp(LispObject * obj);

bool b_listp(LispObject * obj);


// ============================================================================
// Comparison functions
// ============================================================================

bool objs_equal(LispObject * obj1, LispObject * obj2);


// ============================================================================
// Other public functions
// ============================================================================

int test_obj();



#endif
