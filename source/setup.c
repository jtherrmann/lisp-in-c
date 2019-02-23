#include "setup.h"
#include "gc.h"
#include "parse-eval.h"
#include "stack.h"


// ============================================================================
// Private functions
// ============================================================================

// TODO: eval from a source file
void eval_lisp_code() {
    parse_eval("(define not (lambda (x) (cond (x f) (t t))))");
    parse_eval("(define and (lambda (x y) (cond ((not x) x) (t y))))");
    parse_eval("(define or (lambda (x y) (cond ((not x) y) (t x))))");
    parse_eval("(define >= (lambda (x y) (not (< x y))))");
    parse_eval("(define <= (lambda (x y) (not (< y x))))");
    parse_eval("(define > (lambda (x y) (< y x)))");
    parse_eval("(define = (lambda (x y) (not (or (< x y) (< y x)))))");
}


// ============================================================================
// Public functions
// ============================================================================

// This function must be called exactly once. Garbage collection must not be
// triggered for the first time until after this function is called.
void init_setup() {
    stack_ptr = 0;

    weakrefs_head = NULL;
    weakrefs_count = 0;

    make_initial_objs();
    eval_lisp_code();
}
