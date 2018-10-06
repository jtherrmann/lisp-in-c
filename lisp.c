#include <stdio.h>
#include <stdlib.h>

#define INPUT_LEN 2048

// TODO: temp sources list:
// - http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
// - https://carld.github.io/2017/06/20/lisp-in-less-than-200-lines-of-c.html

// TODO: weird indentation
typedef enum {
	      LISP_INT,
	      LISP_CONS
} LispType;

typedef struct LispObject {
    LispType type;

    // TODO: how exactly does this work?
    union {
	// LISP_INT
	int value;

	// LISP_CONS
	struct {
	    struct LispObject * car;
	    struct LispObject * cdr;
	};
    };
} LispObject;


LispObject * lisp_obj(LispType type) {
    LispObject * obj = malloc(sizeof(LispObject));
    // TODO: check for malloc error code?
    obj->type = type;
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
    // TODO: typecheck
    return obj->car;
}


LispObject * lisp_cdr(LispObject * obj) {
    // TODO: typecheck
    return obj->cdr;
}


int main() {

    LispObject * x = lisp_int(1);
    printf("%d\n", x->value);

    LispObject * y = lisp_int(2);
    printf("%d\n", y->value);

    LispObject * c = lisp_cons(x, y);
    printf("%d\n", x);
    printf("%d\n", lisp_car(c));
    printf("%d\n", lisp_car(c)->value);
    printf("%d\n", lisp_cdr(c)->value);

    /* char input[INPUT_LEN]; */
    /* puts("Press Ctrl+c to Exit\n"); */

    /* while (1) { */
    /* 	fputs("> ", stdout); */
    /* 	fgets(input, INPUT_LEN, stdin); */
    /* 	fputs(input, stdout); */
    /* } */

    return 0;
}
