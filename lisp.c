// TODO:
// - address TODO/FIXME throughout files
// - note that in SBCL (consp ()) is NIL but (listp ()) is T, so list and cons
//   must be different types
// - split into multiple files, *.c or *.h as appropriate

// TODO: temp sources list:
// - http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
// - https://carld.github.io/2017/06/20/lisp-in-less-than-200-lines-of-c.html

#include <stdio.h>
#include <stdlib.h>

#define INPUT_LEN 2048
#define LISP_NIL NULL

struct LispObject * weakrefs_head = NULL;

// TODO: weird indentation
typedef enum {
	      LISP_INT,
	      LISP_CONS
} LispType;

typedef struct LispObject {
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

	// LISP_CONS
	struct {
	    struct LispObject * car;
	    struct LispObject * cdr;
	};
    };

    struct LispObject * weakref;

} LispObject;


LispObject * lisp_obj(LispType type) {
    LispObject * obj = malloc(sizeof(LispObject));
    // TODO: check for malloc error code?
    obj->type = type;
    obj->weakref = weakrefs_head;
    weakrefs_head = obj;
    return obj;
}


LispObject * lisp_int(int value) {
    LispObject * obj = lisp_obj(LISP_INT);
    obj->value = value;
}


LispObject * lisp_cons(LispObject * car, LispObject * cdr) {
    LispObject * obj = lisp_obj(LISP_CONS);
    obj->car = car;
    obj->cdr = cdr;
    return obj;
}


LispObject * lisp_car(LispObject * obj) {
    if (obj == LISP_NIL)
	return LISP_NIL;
    // TODO: typecheck
    return obj->car;
}


LispObject * lisp_cdr(LispObject * obj) {
    if (obj == LISP_NIL)
	return LISP_NIL;
    // TODO: typecheck
    return obj->cdr;
}


void lisp_print(LispObject * obj) {
    if (obj == NULL) {
	printf("NIL");
    }
    else {
	// TODO: weird indentation
	switch(obj->type) {

	case LISP_INT:
	    printf("%d", obj->value);
	    break;

	case LISP_CONS:
	    printf("(cons ");
	    lisp_print(obj->car);
	    printf(" ");
	    lisp_print(obj->cdr);
	    printf(")");
	    break;

	default:
	    // TODO: error here
	    printf("ERROR");
	    break;
	}
    }
}


void print_weakrefs() {
    LispObject * current = weakrefs_head;
    while (current != NULL) {
	lisp_print(current);
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


int main() {

    LispObject * x = lisp_int(1);
    /* lisp_print(x); */
    /* printf("\n"); */
    /* printf("%d\n", x->value); */

    LispObject * y = lisp_int(2);
    /* lisp_print(y); */
    /* printf("\n"); */
    /* printf("%d\n", y->value); */

    LispObject * c = lisp_cons(x, y);
    /* lisp_print(c); */
    /* printf("\n"); */
    /* printf("%d\n", x); */
    /* printf("%d\n", lisp_car(c)); */
    /* printf("%d\n", lisp_car(c)->value); */
    /* printf("%d\n", lisp_cdr(c)->value); */

    LispObject * c2 = lisp_cons(x, c);
    /* lisp_print(c2); */
    /* printf("\n"); */

    LispObject * z = lisp_int(3);
    LispObject * c3 = lisp_cons(x, lisp_cons(y, lisp_cons(z, LISP_NIL)));
    /* lisp_print(c3); */
    /* printf("\n"); */

    // creating c3 adds to weakrefs:
    // (cons 1 (cons 2 (cons 3 NIL))) -> (cons 2 (cons 3 NIL)) -> (cons 3 NIL)

    /* LispObject * head = weakrefs_head; */
    /* LispObject * next = weakrefs_head->weakref; */
    /* free(weakrefs_head); */
    /* weakrefs_head = next; */
    print_weakrefs();
    /* lisp_print(head); */

    free_all();

    print_weakrefs();

    /* long i = 1; */
    /* while (1) { */
    /* 	lisp_obj(LISP_INT); */
    /* 	if (i % 100000000 == 0) { */
    /* 	    free_all(); // without this line we get memory leak */
    /* 	    i = 1; */
    /* 	} */
    /* 	++i; */
    /* } */


    /* char input[INPUT_LEN]; */
    /* puts("Press Ctrl+c to Exit\n"); */

    /* while (1) { */
    /* 	fputs("> ", stdout); */
    /* 	fgets(input, INPUT_LEN, stdin); */
    /* 	fputs(input, stdout); */
    /* } */

    return 0;
}
