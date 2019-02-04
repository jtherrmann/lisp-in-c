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

typedef enum {
	      TYPE_INT,
	      TYPE_SYM,
	      TYPE_UNIQUE,
	      TYPE_PAIR,
	      TYPE_LAMBDA,
	      TYPE_BUILTIN_1,
	      TYPE_BUILTIN_2,
	      TYPE_BOOL_BUILTIN_1,
	      TYPE_BOOL_BUILTIN_2
} LispType;


// ============================================================================
// Initial objects
// ============================================================================

// The empty list object.
LispObject * LISP_EMPTY;

// Standard truth symbol.
LispObject * LISP_T;

// Symbols representing special forms.
LispObject * LISP_QUOTE;
LispObject * LISP_COND;
LispObject * LISP_DEFINE;
LispObject * LISP_LAMBDA;

// Builtin eval function.
LispObject * LISP_BUILTIN_EVAL;

// Symbols bound to builtin type predicate functions used by other builtin
// functions to type-check arguments.
LispObject * LISP_PAIR_PRED_SYM;
LispObject * LISP_LIST_PRED_SYM;
LispObject * LISP_INT_PRED_SYM;


// ============================================================================
// LispObject
// ============================================================================

// Adapted and expanded from:
// http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
struct LispObjectStruct {
    LispType type;
    bool is_list;

    union {
	// TYPE_INT
	long value;

	// TYPE_SYM
	char * print_name;

	// TYPE_PAIR
	struct {
	    LispObject * car;
	    LispObject * cdr;
	};

	// TYPE_LAMBDA
	struct {
	    LispObject * args;
	    LispObject * body;
	    LispObject * env_list;
	};

	struct {
	    LispObject * builtin_name;
	    union{
		// TYPE_BUILTIN_1
		LispObject * (* b_func_1)(LispObject *);

		// TYPE_BUILTIN_2
		LispObject * (* b_func_2)(LispObject *, LispObject *);

		// TYPE_BOOL_BUILTIN_1
		bool (* b_bool_func_1)(LispObject *);

		// TYPE_BOOL_BUILTIN_2
		bool (* b_bool_func_2)(LispObject *, LispObject *);
	    };
	};
    };

    bool marked;
    LispObject * weakref;
};

void make_initial_objs();


// ----------------------------------------------------------------------------
// Public constructors
// ----------------------------------------------------------------------------

LispObject * get_int(long value);

LispObject * get_sym_by_substr(char * str, long begin, long end);

LispObject * get_lambda(LispObject * args, LispObject * body, LispObject * env_list);

LispObject * b_cons(LispObject * car, LispObject * cdr);


// ============================================================================
// car, cdr, and length
// ============================================================================

LispObject * b_car(LispObject * obj);

LispObject * b_cdr(LispObject * obj);

LispObject * car(LispObject * obj);

LispObject * cdr(LispObject * obj);

LispObject * b_length(LispObject * obj);

long length(LispObject * obj);


// ============================================================================
// Type predicates
// ============================================================================

bool b_null_pred(LispObject * obj);

bool b_int_pred(LispObject * obj);

bool b_symbol_pred(LispObject * obj);

bool b_pair_pred(LispObject * obj);

bool b_list_pred(LispObject * obj);

bool b_function_pred(LispObject * obj);

bool is_builtin(LispObject * obj);


#endif
