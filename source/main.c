#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "env.h"
#include "eval.h"
#include "gc.h"
#include "error.h"
#include "obj.h"
#include "parse.h"
#include "print.h"
#include "stack.h"


// bad_stack
// Handle an invalid stack state.
void bad_stack() {
    printf("\nAbort! Stack pointer is %ld but should be 0.\nStack contents: ", sp);
    print_stack();
    printf("\n");
    FOUND_BUG;
}


// process_input
// Parse and evaluate an input expression and print the result.
void process_input(char * input_str) {
    input = input_str;

    // Stores the return values of parse and eval.
    LispObject * obj;

    input_index = 0;
    skipspace();  // Meet parse's pre.

    if (input[input_index] != INPUT_END && input[input_index] != ';') {
	obj = parse();

	if (sp != 0)
	    bad_stack();

	if (obj != NULL
	    && input[input_index] != INPUT_END
	    && input[input_index] != ';') {

	    show_input_char();
	    printf("%sexpected end of input but got '%c'\n",
		   PARSE_ERR, input[input_index]);
	    obj = NULL;
	}

	if (obj != NULL) {
	    // Meet eval's pre by protecting its first arg from GC.
	    push(obj);

	    // LISP_EMPTY is part of the initial set of objects protected
	    // from GC, so it meets eval's pre that its second arg is
	    // protected from GC.
	    obj = eval(obj, LISP_EMPTY);

	    pop();

	    if (sp != 0)
		bad_stack();

	    if (obj != NULL) {
		print_obj(obj);
		printf("\n");
	    }
	}
    }
}


// TODO: eval from a source file
void eval_lisp_code() {
    process_input("(define not (lambda (x) (cond (x f) (t t))))");
    process_input("(define and (lambda (x y) (cond ((not x) x) (t y))))");
    process_input("(define or (lambda (x y) (cond ((not x) y) (t x))))");
    process_input("(define >= (lambda (x y) (not (< x y))))");
    process_input("(define <= (lambda (x y) (not (< y x))))");
    process_input("(define > (lambda (x y) (< y x)))");
    process_input("(define = (lambda (x y) (not (or (< x y) (< y x)))))");
}


void init_setup() {
    // Initialize the stack pointer.
    sp = 0;

    weakrefs_head = NULL;
    weakrefs_count = 0;

    make_initial_objs();
    eval_lisp_code();
}


int main() {
    init_setup();

    printf("Welcome to Lisp!\n");
    printf("Exit with Ctrl-c\n\n");

    while (true) {
	input = readline("> ");
	add_history(input);
	process_input(input);
	free(input);
    }
    FOUND_BUG;
}
