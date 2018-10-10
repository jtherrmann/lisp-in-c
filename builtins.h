// builtins.h
// Header for Lisp builtins.


#ifndef BUILTINS_H
#define BUILTINS_H


#include <stdbool.h>
#include <stdlib.h>


// ============================================================================
// typedefs
// ============================================================================

typedef struct LispObjectStruct LispObject;

// TODO: weird indentation
typedef enum {
	      LISP_INT,
	      LISP_CONS,
	      LISP_NILTYPE
} LispType;


// ============================================================================
// Global variables
// ============================================================================

LispObject * LISP_NIL;


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

	// LISP_CONS and LISP_NILTYPE
	struct {
	    LispObject * car;
	    LispObject * cdr;
	};
    };

    LispObject * weakref;

};


// ----------------------------------------------------------------------------
// Internal constructors
// ----------------------------------------------------------------------------

LispObject * lisp_nil();

LispObject * lisp_int(int value);


// ----------------------------------------------------------------------------
// cons, car, and cdr
// ----------------------------------------------------------------------------

LispObject * lisp_cons(LispObject * car, LispObject * cdr);

LispObject * lisp_car(LispObject * obj);

LispObject * lisp_cdr(LispObject * obj);


// ============================================================================
// Type predicates
// ============================================================================

bool lisp_null(LispObject * object);

bool lisp_numberp(LispObject * object);

bool lisp_consp(LispObject * object);

bool lisp_listp(LispObject * object);


#endif
