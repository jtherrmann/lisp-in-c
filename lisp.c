// TODO:
// - address TODO/FIXME throughout files
// - note that in SBCL (consp ()) is NIL but (listp ()) is T, so list and cons
//   must be different types
// - split into multiple files, *.c or *.h as appropriate
// - temp sources list:
//   - http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
//   - https://carld.github.io/2017/06/20/lisp-in-less-than-200-lines-of-c.html
// - replace checks of ->type and == LISP_NIL with type predicates like atom,
//   null, consp, listp, symbolp, etc.
// - replace uses of 0 and 1 as bools with false/true

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


// ============================================================================
// Macros
// ============================================================================

#define INPUT_LEN 2048
#define INPUT_END '\n'
#define isdigit(ch) (ch >= '0' && ch <= '9')


// ============================================================================
// Global variables
// ============================================================================

char input[INPUT_LEN];
int input_index;

struct LispObject * weakrefs_head = NULL;
struct LispObject * LISP_NIL;


// ============================================================================
// Lisp
// ============================================================================

// TODO: weird indentation
typedef enum {
	      LISP_INT,
	      LISP_CONS,
	      LISP_NILTYPE
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


bool lisp_null(LispObject * object) {
    return object == LISP_NIL;
}


bool lisp_numberp(LispObject * object) {
    return object->type == LISP_INT;
}


bool lisp_consp(LispObject * object) {
    return object->type == LISP_CONS;
}


bool lisp_listp(LispObject * object) {
    return lisp_consp(object) || lisp_null(object);
}


LispObject * lisp_obj(LispType type) {
    LispObject * obj = malloc(sizeof(LispObject));
    // TODO: check for malloc error code?
    obj->type = type;
    obj->weakref = weakrefs_head;
    weakrefs_head = obj;
    return obj;
}


LispObject * lisp_nil() {
    LispObject * obj = lisp_obj(LISP_NILTYPE);
    obj->car = obj;
    obj->cdr = obj;
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
    // TODO: proper typecheck
    assert(lisp_listp(obj));
    return obj->car;
}


LispObject * lisp_cdr(LispObject * obj) {
    // TODO: proper typecheck
    assert(lisp_listp(obj));
    return obj->cdr;
}


// ============================================================================
// Parse
// ============================================================================

LispObject * parseint();
LispObject * parselist();
LispObject * parsesym();
int power(int, int);
void skipspace();


// parse
// Convert part of the input str to a Lisp object.
//
// Pre:
// - input[input_index] is a non-space char.
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
LispObject * parse() {
    if (isdigit(input[input_index]))
	return parseint();  // parseint fulfills parse's post.

    if (input[input_index] == '(') {
	// Skip the '(' to start at the first element of the list.
	input_index += 1;
	skipspace();  // Meet parselist's pre.
	return parselist();  // parselist fulfills parse's post.
    }

    // TODO: support ' (syntax for quote) and "" (syntax for str)
    if (input[input_index] != ')'
	&& input[input_index] != '\''
	&& input[input_index] != '"')
	return parsesym();  // parsesym fulfills parse's post.
    
    printf("PARSE ERROR: unrecognized char\n");
    exit(1);
}


// TODO: handle overflow; also, make ints as large as possible (e.g. signed
// longs or whatever)
//
// parseint
// Convert part of the input str to a Lisp int.
//
// Pre:
// - input[input_index] is a char in the range '0'-'9'.
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
LispObject * parseint() {

    // Go to the end of the substr that represents the int.
    int start = input_index;
    while (input[input_index] != '('
	   && input[input_index] != ')'
	   && input[input_index] != ' '
	   && input[input_index] != INPUT_END) {
    	if (!isdigit(input[input_index])) {
    	    printf("PARSE ERROR: non-digit char in number\n");
	    exit(1);
	}
    	++input_index;
    }
    int end = input_index;
    --input_index;

    // Go back through the substr, adding up the values of the digits to get
    // the overall value of the int.
    int total = 0;
    int place = 0;
    int digit;
    while (input_index >= start) {
	digit = input[input_index] - 0x30;
	total += digit * power(10, place);
	++place;
	--input_index;
    }

    // Fulfill post.
    input_index = end;
    skipspace();

    return lisp_int(total);
}


// power
// Return b to the power of n.
//
// Pre:
// - n >= 0
int power(int b, int n) {
    if (n == 0)
	return 1;

    int total = b;
    while (n > 1) {
	total *= b;
	--n;
    }
    return total;
}


// parsesym
// Convert part of the input str to a Lisp symbol.
//
// Pre:
// - input[input_index] is a non-space char that is not (, ), ', ", or a digit
//   in the range 0-9.
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
LispObject * parsesym() {

    // Go to the end of the substr that represents the symbol, in order to
    // validate it and get its length.
    int start = input_index;
    while (input[input_index] != '('
	   && input[input_index] != ')'
	   && input[input_index] != ' '
	   && input[input_index] != INPUT_END) {
	if (input[input_index] == '\'' || input[input_index] == '"') {
	    printf("PARSE ERROR: invalid char in symbol\n");
	    exit(1);
	}
	++input_index;
    }
    int end = input_index;
    input_index = start;
    // TODO: may change how start, end, and input_index are used here

    // TODO: a LispObject of type LISP_SYM stores its print name and the length
    // of its print name; here, check if the substr denoted by start
    // (inclusive) and end (exclusive) is equal to the print name of any symbol
    // in the symbol interns list (by first comparing lens and then comparing
    // chars if lens are equal) and if it is, return the interned symbol; if
    // it's not, create a new symbol, add it onto the front of the interns
    // list, and return it
    // - in lisp_obj, if the type is LISP_SYM, add the obj to the symbol
    //   interns list rather than the weakrefs list; then sweep the interns
    //   list for unmarked symbols during GC

    printf("PARSE ERROR: parsesym not implemented\n");
    exit(1);
}


// parselist
// Convert part of the input str to a Lisp list.
//
// Pre:
// - input[input_index] is a non-space char.
//
// Post:
// - input[input_index] is the first non-space char after the parsed substr.
LispObject * parselist() {
    if (input[input_index] == ')') {
	// Fulfill post.
	++input_index;
	skipspace();

	// Return the empty list object.
	return LISP_NIL;
    }

    if (input[input_index] == INPUT_END) {
	printf("PARSE ERROR: incomplete list\n");
	exit(1);
    }

    // Invariant: pre still true.

    LispObject * car = parse(input);  // parselist's pre meets parse's pre.
    LispObject * cdr = parselist(input);
    return lisp_cons(car, cdr);
}


void skipspace() {
    while (input[input_index] == ' ')
	++input_index;
}


// ============================================================================
// Print
// ============================================================================

void print_list(LispObject * obj);

void print_obj(LispObject * obj) {
    if (lisp_null(obj)) {
	printf("NIL");
    }
    else {
	// TODO: weird indentation
	switch(obj->type) {

	case LISP_INT:
	    printf("%d", obj->value);
	    break;

	case LISP_CONS:
	    // TODO: print lists properly but maybe leave this version in as a
	    // debug option
	    /* printf("(cons "); */
	    /* print_obj(lisp_car(obj)); */
	    /* printf(" "); */
	    /* print_obj(lisp_cdr(obj)); */
	    /* printf(")"); */
	    print_list(obj);
	    break;

	default:
	    printf("PRINT ERROR: unrecognized type\n");
	    exit(1);
	}
    }
}


// print_list
// Print a Lisp list.
//
// Pre:
// - obj->type == LISP_CONS
void print_list(LispObject * obj) {
    printf("(");
    while (true) {
	print_obj(lisp_car(obj));
	obj = lisp_cdr(obj);
	if (obj->type != LISP_CONS)
	    break;
	printf(" ");
    }
    if (!lisp_null(obj)) {
	printf(" . ");
	print_obj(obj);
    }
    printf(")");
}


// ============================================================================
// Debugging
// ============================================================================

// TODO: remove these when they're no longer needed or make them available as
// special interpreter commands

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


// ============================================================================
// Tests
// ============================================================================

bool objs_equal(LispObject * obj1, LispObject * obj2);


void test_power() {
    assert(power(10, 0) == 1);
    assert(power(10, 1) == 10);
    assert(power(10, 2) == 100);
    assert(power(10, 3) == 1000);

    assert(power(-10, 0) == 1);
    assert(power(-10, 1) == -10);
    assert(power(-10, 2) == 100);
    assert(power(-10, 3) == -1000);

    assert(power(-15, 3) == -3375);
    assert(power(-12, 4) == 20736);
    assert(power(-6, 6) == 46656);
    assert(power(-4, 7) == -16384);

    assert(power(2, 12) == 4096);
    assert(power(3, 3) == 27);
    assert(power(5, 3) == 125);
    assert(power(6, 5) == 7776);
    assert(power(12, 4) == 20736);
    assert(power(15, 3) == 3375);
}


void test_objs_equal() {
    LispObject * obj1;
    LispObject * obj2;

    obj1 = lisp_int(1);
    obj2 = LISP_NIL;
    assert(!objs_equal(obj1, obj2));

    obj1 = LISP_NIL;
    obj2 = LISP_NIL;
    assert(objs_equal(obj1, obj2));

    obj1 = lisp_int(3);
    obj2 = lisp_int(5);
    assert(!objs_equal(obj1, obj2));

    obj1 = lisp_int(3);
    obj2 = lisp_int(3);
    assert(objs_equal(obj1, obj2));

    obj1 = lisp_cons(lisp_int(1), LISP_NIL);
    obj2 = lisp_cons(lisp_int(2), LISP_NIL);
    assert(!objs_equal(obj1, obj2));

    obj1 = lisp_cons(lisp_int(1), LISP_NIL);
    obj2 = lisp_cons(lisp_int(1), LISP_NIL);
    assert(objs_equal(obj1, obj2));

    obj1 = lisp_cons(lisp_int(5), lisp_int(10));
    obj2 = lisp_cons(lisp_int(3), lisp_int(10));
    assert(!objs_equal(obj1, obj2));

    obj1 = lisp_cons(lisp_int(5), lisp_int(10));
    obj2 = lisp_cons(lisp_int(5), lisp_int(10));
    assert(objs_equal(obj1, obj2));

    obj1 = lisp_cons(lisp_int(1),
		     lisp_cons(lisp_int(2),
			       lisp_cons(lisp_int(3), lisp_int(3))));
    obj2 = lisp_cons(lisp_int(1),
		     lisp_cons(lisp_int(2),
			       lisp_cons(lisp_int(3), LISP_NIL)));
    assert(!objs_equal(obj1, obj2));

    obj1 = lisp_cons(lisp_int(1),
		     lisp_cons(lisp_int(2),
			       lisp_cons(lisp_int(3), LISP_NIL)));
    obj2 = lisp_cons(lisp_int(1),
		     lisp_cons(lisp_int(2),
			       lisp_cons(lisp_int(3), LISP_NIL)));
    assert(objs_equal(obj1, obj2));

    obj2 = LISP_NIL;
    assert(!objs_equal(obj1, obj2));

    obj2 = obj1;
    assert(objs_equal(obj1, obj2));
}


void run_tests() {
    printf("Running tests.\n");

    test_power();
    test_objs_equal();

    printf("All tests pass.\n\n");
}


// ----------------------------------------------------------------------------
// Test utilities
// ----------------------------------------------------------------------------

bool objs_equal(LispObject * obj1, LispObject * obj2) {
    if (obj1 == obj2)
	return true;

    if (obj1->type != obj2->type)
	return false;

    switch (obj1->type) {

    case LISP_INT:
	return obj1->value == obj2->value;

    case LISP_CONS:
	return objs_equal(lisp_car(obj1), lisp_car(obj2))
	    && objs_equal(lisp_cdr(obj1), lisp_cdr(obj2));

    default:
	printf("COMPARISON ERROR: unrecognized type\n");
	exit(1);
    }
}


// ============================================================================
// Main
// ============================================================================

int main() {

    /* LispObject * x = lisp_int(1); */
    /* print_obj(x); */
    /* printf("\n"); */
    /* printf("%d\n", x->value); */

    /* LispObject * y = lisp_int(2); */
    /* print_obj(y); */
    /* printf("\n"); */
    /* printf("%d\n", y->value); */

    /* LispObject * c = lisp_cons(x, y); */
    /* print_obj(c); */
    /* printf("\n"); */
    /* printf("%d\n", x); */
    /* printf("%d\n", lisp_car(c)); */
    /* printf("%d\n", lisp_car(c)->value); */
    /* printf("%d\n", lisp_cdr(c)->value); */

    /* LispObject * c2 = lisp_cons(x, c); */
    /* print_obj(c2); */
    /* printf("\n"); */

    /* LispObject * z = lisp_int(3); */
    /* LispObject * c3 = lisp_cons(x, lisp_cons(y, lisp_cons(z, LISP_NIL))); */
    /* print_obj(c3); */
    /* printf("\n"); */

    // creating c3 adds to weakrefs:
    // (cons 1 (cons 2 (cons 3 NIL))) -> (cons 2 (cons 3 NIL)) -> (cons 3 NIL)

    /* LispObject * head = weakrefs_head; */
    /* LispObject * next = weakrefs_head->weakref; */
    /* free(weakrefs_head); */
    /* weakrefs_head = next; */
    /* print_weakrefs(); */
    /* print_obj(head); */

    /* free_all(); */

    /* print_weakrefs(); */

    /* long i = 1; */
    /* while (1) { */
    /* 	lisp_obj(LISP_INT); */
    /* 	if (i % 100000000 == 0) { */
    /* 	    free_all(); // without this line we get memory leak */
    /* 	    i = 1; */
    /* 	} */
    /* 	++i; */
    /* } */


    /* LispObject * x = lisp_int(1); */
    /* LispObject * y = lisp_int(2); */
    /* LispObject * c = lisp_cons(x, lisp_cons(x, y)); */
    /* LispObject * c = lisp_cons(x, lisp_cons(lisp_cons(x, y), lisp_cons(x, LISP_NIL))); */
    /* print_obj(c); */
    /* printf("\n"); */

    LISP_NIL = lisp_nil();

    // TODO: add to tests
    assert(lisp_car(LISP_NIL) == LISP_NIL && lisp_cdr(LISP_NIL) == LISP_NIL);

    run_tests();

    printf("Welcome to Lisp!\n");
    printf("Exit with Ctrl-c\n\n");

    while (1) {
    	fputs("> ", stdout);
    	fgets(input, INPUT_LEN, stdin);

	input_index = 0;
	skipspace();  // Meet parse's pre.

	if (input[input_index] != INPUT_END) {
	    LispObject * obj = parse();

	    if (input[input_index] != INPUT_END) {
		printf("PARSE ERROR: multiple expressions\n");
		exit(1);
	    }

	    print_obj(obj);
	    printf("\n");
	}
    }

    return 0;
}
