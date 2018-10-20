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
	      TYPE_INT,
	      TYPE_SYM,
	      TYPE_UNIQUE,
	      TYPE_CONS,
	      TYPE_FUNC,
	      TYPE_BUILTIN_1,
	      TYPE_BUILTIN_2
} LispType;


// ============================================================================
// Initial objects
// ============================================================================

// The empty list object NIL.
LispObject * LISP_NIL;

// Boolean objects.
LispObject * LISP_T;
LispObject * LISP_F;

// Boolean symbols.
LispObject * LISP_T_SYM;
LispObject * LISP_F_SYM;

// Symbols representing special forms.
LispObject * LISP_QUOTE;
LispObject * LISP_DEF;
LispObject * LISP_LAMBDA;


// ============================================================================
// LispObject
// ============================================================================

// Adapted and expanded from:
// http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
struct LispObjectStruct {
    LispType type;

    union {
	// TYPE_INT
	int value;

	// TYPE_SYM
	char * print_name;

	// TYPE_CONS and LISP_NIL
	struct {
	    LispObject * car;
	    LispObject * cdr;
	};

	// TYPE_FUNC
	struct {
	    LispObject * args;
	    LispObject * body;
	};

	// TYPE_BUILTIN_1
	LispObject * (* b_func_1)(LispObject *);

	// TYPE_BUILTIN_2
	LispObject * (* b_func_2)(LispObject *, LispObject *);
    };

    bool marked;
    LispObject * weakref;
};


void make_initial_objs();


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

LispObject * get_func(LispObject * args, LispObject * body);

LispObject * get_builtin_1(LispObject * (* b_func_1)(LispObject *));

LispObject * get_builtin_2(LispObject * (* c_func)(LispObject *, LispObject *));

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

bool b_funcp(LispObject * obj);

bool b_builtinp(LispObject * obj);


// ============================================================================
// Comparison functions
// ============================================================================

bool b_equal(LispObject * obj1, LispObject * obj2);


#endif
