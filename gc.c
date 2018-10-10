// gc.c
// Source for garbage collector.


#include <stdio.h>

#include "gc.h"


// ============================================================================
// Debugging
// ============================================================================

void print_weakrefs() {
    LispObject * current = weakrefs_head;
    while (current != NULL) {
	print_obj(current);
	printf(" -> ");
	current = current->weakref;
    }
    printf("NULL\n");
}


void free_all() {
    while (weakrefs_head != NULL) {
	LispObject * next = weakrefs_head->weakref;
	free(weakrefs_head);
	weakrefs_head = next;
    }
}
