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
	      TYPE_BUILTIN_2,
	      TYPE_BOOL_BUILTIN_1,
	      TYPE_BOOL_BUILTIN_2,
	      TYPE_BUILTIN_1_ENV
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
LispObject * LISP_COND;
LispObject * LISP_DEF;
LispObject * LISP_LAMBDA;

// Symbols bound to builtin type predicate functions used by other builtin
// functions to type-check arguments.
LispObject * LISP_BOOL_PRED_SYM;
LispObject * LISP_LIST_PRED_SYM;


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
	    bool empty_last;
	};

	// TYPE_FUNC
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

		// TYPE_BUILTIN_1_ENV
		LispObject * (* b_func_1_env)(LispObject *, LispObject *);
	    };
	};
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

LispObject * get_func(LispObject * args, LispObject * body, LispObject * env_list);

LispObject * get_builtin_1(LispObject * builtin_name,
			   LispObject * (* b_func_1)(LispObject *));

LispObject * get_builtin_2(LispObject * builtin_name,
			   LispObject * (* b_func_2)(LispObject *, LispObject *));

LispObject * get_bool_builtin_1(LispObject * builtin_name,
				bool (* b_bool_func_1)(LispObject *));

LispObject * get_bool_builtin_2(LispObject * builtin_name,
				bool (* b_bool_func_2)(LispObject *, LispObject *));

LispObject * get_builtin_1_env(LispObject * builtin_name,
		  LispObject * (* b_func_1_env)(LispObject *, LispObject *));


// ----------------------------------------------------------------------------
// cons, car, and cdr
// ----------------------------------------------------------------------------

LispObject * b_cons(LispObject * car, LispObject * cdr);

LispObject * b_car_2(LispObject * obj);

LispObject * b_cdr_2(LispObject * obj);

LispObject * b_car(LispObject * obj);

LispObject * b_cdr(LispObject * obj);


// ============================================================================
// Type predicates
// ============================================================================

bool b_null_pred(LispObject * obj);

bool b_bool_pred(LispObject * obj);

bool b_number_pred(LispObject * obj);

bool b_symbol_pred(LispObject * obj);

bool b_cons_pred(LispObject * obj);

bool b_list_pred(LispObject * obj);

bool b_func_pred(LispObject * obj);

bool b_builtin_pred(LispObject * obj);


#endif
