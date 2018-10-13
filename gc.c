// gc.c
// Source for garbage collector.


// TODO: sources


#include <stdio.h>

#include "env.h"
#include "gc.h"
#include "print.h"


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

// TODO: include stack in root set, update doc
//
// mark
// Mark objects reachable from the root set (the global environment).
void mark() {
    struct binding * b;
    for (int i = 0; i < HASHSIZE; ++i) {
	for (b = env[i]; b != NULL; b = b->next) {
	    mark_obj(b->name);
	    mark_obj(b->def);
	}
    }
}


// mark_obj
// Mark an object as reachable.
void mark_obj(LispObject * obj) {
    if (!b_null(obj)) {

	// If obj is a cons and is already marked then we don't mark its car
	// and cdr because they must already have been marked when obj was
	// marked. Without this check, marking recurses infinitely if there are
	// any circular references reachable from the root set; for example, if
	// the cdr of obj is obj.
	if (b_consp(obj) && !obj->marked) {
	    obj->marked = true;
	    mark_obj(b_car(obj));
	    mark_obj(b_cdr(obj));
	}
	else
	    obj->marked = true;
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
    sweep();
}


// ----------------------------------------------------------------------------
// Debugging
// ----------------------------------------------------------------------------

void print_weakrefs() {
    LispObject * current = weakrefs_head;
    while (current != NULL) {
	print_obj(current);
	printf(" -> ");
	current = current->weakref;
    }
    printf("NULL\n\n");
    printf("weakrefs count: %d\n", weakrefs_count);
}


void free_all() {
    while (weakrefs_head != NULL) {
	LispObject * next = weakrefs_head->weakref;
	free(weakrefs_head);
	weakrefs_head = next;
    }
}
