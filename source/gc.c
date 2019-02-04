// gc.c
// Source for garbage collector.


// Sources:
// - https://github.com/marcpaq/arpilisp
// - http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
// - https://courses.cs.washington.edu/courses/csep521/07wi/prj/rick.pdf
// - https://stackoverflow.com/a/30081106


#include <stdio.h>

#include "env.h"
#include "gc.h"
#include "error.h"
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
// Mark the initial set of objects and objects reachable from the global
// environment or the stack.
void mark() {
    mark_obj(LISP_EMPTY);
    mark_obj(LISP_T);

    mark_obj(LISP_QUOTE);
    mark_obj(LISP_COND);
    mark_obj(LISP_DEFINE);
    mark_obj(LISP_LAMBDA);

    struct binding * b;
    for (long i = 0; i < ENV_SIZE; ++i) {
	for (b = global_env[i]; b != NULL; b = b->next) {
	    mark_obj(b->name);
	    mark_obj(b->def);
	}
    }

    for (long i = sp; i > 0; --i)
	mark_obj(stack[i]);
}


// mark_obj
// Mark an object as reachable.
void mark_obj(LispObject * obj) {
    // Don't mark obj if it's already marked. Without this check, marking
    // recurses infinitely if there are any circular references reachable from
    // obj; for example, if obj is a pair and the cdr of obj is obj.
    if (!obj->marked) {

	if (gc_output) {
	    printf("mark: ");
	    print_obj(obj);
	    printf("\n");
	}

	obj->marked = true;

	if (b_pair_pred(obj)) {
	    mark_obj(car(obj));
	    mark_obj(cdr(obj));
	}
	else if (obj->type == TYPE_LAMBDA) {
	    mark_obj(obj->args);
	    mark_obj(obj->body);
	    mark_obj(obj->env_list);
	}
	else if (is_builtin(obj))
	    mark_obj(obj->builtin_name);
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
    if (weakrefs_count <= 0)
	FOUND_BUG;

    if (gc_output) {
	printf("free: ");
	print_obj(obj);
	printf("\n");
    }

    if (b_symbol_pred(obj))
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

