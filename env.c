// env.c
// Source for the global Lisp environment.


// The hash table implementation is adapted from K&R C, 2nd ed., pp. 143-45.


#include <assert.h>
#include <stdio.h>

#include "env.h"
#include "print.h"


// ============================================================================
// Private function prototypes
// ============================================================================

unsigned hash(LispObject * sym);
struct binding * lookup(LispObject * sym, unsigned hashval);


// ============================================================================
// Public functions
// ============================================================================

// bind
// Bind a name to a definition.
//
// Pre:
// - b_symbol_pred(sym)
void bind(LispObject * sym, LispObject * def) {
    unsigned hashval = hash(sym);
    struct binding * b = lookup(sym, hashval);
    if (b == NULL) {
	b = malloc(sizeof(struct binding));
	b->next = global_env[hashval];
	global_env[hashval] = b;
    }
    b->name = sym;
    b->def = def;
}


// get_def
// Return the definition bound to the given name.
//
// Pre:
// - b_symbol_pred(sym)
LispObject * get_def(LispObject * sym) {
    unsigned hashval = hash(sym);
    struct binding * b = lookup(sym, hashval);
    // TODO: proper error
    assert(b != NULL);
    return b->def;
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
    unsigned hashval = 0;
    int i = 0;
    while (sym->print_name[i] != '\0') {
	hashval = sym->print_name[i] + 31 * hashval;
	++i;
    }
    return hashval % HASHSIZE;
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
	if (b_equal(sym, b->name))
	    return b;
    return NULL;
}

