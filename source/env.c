// env.c
// Source for the global Lisp environment.


// The hash table implementation is adapted from K&R C, 2nd ed., pp. 143-45.


#include <stdio.h>

#include "env.h"
#include "builtins.h"
#include "hash.h"
#include "print.h"


// ============================================================================
// Private function prototypes
// ============================================================================

unsigned hash(LispObject * sym);

struct binding * lookup(LispObject * sym, unsigned hashval);

void print_env(bool print_indices);


// ============================================================================
// Public functions
// ============================================================================

// bind
// Bind a name to a definition.
//
// Pre:
// - b_symbol_pred(sym)
//
// On error:
// - Return false.
bool bind(LispObject * sym, LispObject * def, bool constant) {
    unsigned hashval = hash(sym);
    struct binding * b = lookup(sym, hashval);
    if (b == NULL) {
	b = malloc(sizeof(struct binding));
	b->next = global_env[hashval];
	global_env[hashval] = b;
    }
    else if (b->constant)
	return false;

    b->name = sym;
    b->def = def;
    b->constant = constant;
    return true;
}


// get_def
// Return the definition bound to the given name, or NULL if the name is
// undefined.
//
// Pre:
// - b_symbol_pred(sym)
LispObject * get_def(LispObject * sym) {
    unsigned hashval = hash(sym);
    struct binding * b = lookup(sym, hashval);
    if (b == NULL)
	return NULL;
    return b->def;
}


LispObject * b_print_env(LispObject * indices) {
    print_env(to_bool(indices));
    return LISP_EMPTY;
}


// ============================================================================
// Private functions
// ============================================================================

// hash
// Hash a symbol.
//
// Pre:
// - b_symbol_pred(sym)
unsigned hash(LispObject * sym) {
    return hash_string(sym->print_name) % ENV_SIZE;
}


// lookup
// Return the binding for the given symbol and hash value, or NULL if the
// binding does not exist.
//
// Pre:
// - b_symbol_pred(sym)
// - hashval == hash(sym)
struct binding * lookup(LispObject * sym, unsigned hashval) {
    for (struct binding * b = global_env[hashval]; b != NULL; b = b->next)
	if (b_equal_pred(sym, b->name))
	    return b;
    return NULL;
}


// print_env
// Print the global environment.
void print_env(bool print_indices) {
    struct binding * b;
    for (long i = 0; i < ENV_SIZE; ++i) {
	if (print_indices && global_env[i] != NULL) {
	    printf("---\n");
	    printf("%ld\n", i);
	    printf("---\n");
	}
	for (b = global_env[i]; b != NULL; b = b->next) {
	    print_obj(b->name);
	    printf("\n");
	    print_obj(b->def);
	    printf("\n");
	    printf("\n");
	}
    }
}
