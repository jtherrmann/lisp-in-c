// gc.c
// Source for garbage collector.


// Sources:
// - https://github.com/marcpaq/arpilisp
// - http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
// - https://courses.cs.washington.edu/courses/csep521/07wi/prj/rick.pdf
// - https://stackoverflow.com/a/30081106


#include <assert.h>
#include <stdio.h>

#include "env.h"
#include "gc.h"
#include "print.h"
#include "stack.h"


// ============================================================================
// Private function prototypes
// ============================================================================

void mark();

void mark_obj(LispObject * obj);

void sweep();

void free_obj(LispObject * obj);


// ============================================================================
// Private functions
// ============================================================================

// mark
// Mark NIL, symbols representing special forms, and objects reachable from the
// global environment or the stack.
void mark() {

    // NIL
    mark_obj(LISP_NIL);

    // Special forms.
    mark_obj(LISP_QUOTE);
    mark_obj(LISP_DEF);
    mark_obj(LISP_LAMBDA);

    // Global env.
    struct binding * b;
    for (int i = 0; i < HASHSIZE; ++i) {
	for (b = env[i]; b != NULL; b = b->next) {
	    mark_obj(b->name);
	    mark_obj(b->def);
	}
    }

    // Stack.
    for (int i = sp; i > 0; --i)
	mark_obj(stack[i]);
}


// mark_obj
// Mark an object as reachable.
void mark_obj(LispObject * obj) {
    // Don't mark obj if it's already marked. Without this check, marking
    // recurses infinitely if there are any circular references reachable from
    // obj; for example, if obj is a cons and the cdr of obj is obj.
    if (!obj->marked) {

	if (gc_output) {
	    printf("mark: ");
	    print_obj(obj);
	    printf("\n");
	}

	obj->marked = true;

	if (b_consp(obj)) {
	    mark_obj(b_car(obj));
	    mark_obj(b_cdr(obj));
	}
	else if (b_funcp(obj)) {
	    mark_obj(obj->args);
	    mark_obj(obj->body);
	}
    }
}


// sweep
// Sweep the weak references, freeing unmarked objects and unmarking marked
// objects.
void sweep() {
    LispObject * temp;

    // Pop the head off the weak refs list until the list is empty or the head
    // is marked.
    while (weakrefs_head != NULL && !weakrefs_head->marked) {
	temp = weakrefs_head->weakref;
	free_obj(weakrefs_head);
	weakrefs_head = temp;
    }

    // If the weak refs list is not empty, unmark the head and then iterate
    // through the remaining items.
    if (weakrefs_head != NULL) {
	weakrefs_head->marked = false;

	LispObject * current = weakrefs_head;
	while (current->weakref != NULL) {
	    if (!current->weakref->marked) {  // Free unmarked object.
		temp = current->weakref->weakref;
		free_obj(current->weakref);
		current->weakref = temp;
	    }
	    else {  // Unmark marked object.
		current = current->weakref;
		current->marked = false;
	    }
	}
    }
}


// free_obj
// Free a Lisp object.
void free_obj(LispObject * obj) {
    assert(weakrefs_count > 0);

    if (gc_output) {
	printf("free: ");
	print_obj(obj);
	printf("\n");
    }

    if (b_symbolp(obj))
	free(obj->print_name);

    free(obj);
    --weakrefs_count;
}

	

// ============================================================================
// Public functions
// ============================================================================

// collect_garbage
// Mark reachable objects and then free unmarked objects.
void collect_garbage() {
    mark();

    if (gc_output)
	printf("\n");

    sweep();

    if (gc_output)
	printf("\n");
}


// ----------------------------------------------------------------------------
// Debugging
// ----------------------------------------------------------------------------

// TODO: remove when unneeded
void free_all() {
    while (weakrefs_head != NULL) {
	LispObject * next = weakrefs_head->weakref;
	free(weakrefs_head);
	weakrefs_head = next;
    }
}
