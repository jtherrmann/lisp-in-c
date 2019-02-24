#include <stdio.h>

#include "builtins.h"
#include "obj.h"
#include "error.h"
#include "parse-eval.h"
#include "setup.h"


// TODO: assure GC doesn't run at all during tests, then run them all again w/
// GC running every time


void test_parse_eval_positive_ints() {
    ASSERT(b_equal_pred(parse_eval("0"), get_int(0)));
    ASSERT(b_equal_pred(parse_eval("1"), get_int(1)));
    ASSERT(b_equal_pred(parse_eval("123"), get_int(123)));
    ASSERT(b_equal_pred(parse_eval("981723"), get_int(981723)));
}


void test_parse_eval_negative_ints() {
    ASSERT(b_equal_pred(parse_eval("-1"), get_int(-1)));
    ASSERT(b_equal_pred(parse_eval("-425"), get_int(-425)));
    ASSERT(b_equal_pred(parse_eval("-871312"), get_int(-871312)));
}


// TODO: remove this test after adding tests for quote with each object type
void test_parse_eval_quoted_symbols() {
    ASSERT(b_equal_pred(parse_eval("(quote x)"), get_sym("x")));
    ASSERT(b_equal_pred(parse_eval("(quote foo)"), get_sym("foo")));
}


void test_parse_eval_undefined_symbols() {
    ASSERT(parse_eval("x") == NULL);
    ASSERT(parse_eval("foo") == NULL);
}


void test_parse_eval_defined_symbols() {
    ASSERT(parse_eval("test-defined-symbol") == NULL);
    parse_eval("(define test-defined-symbol 500)");
    ASSERT(b_equal_pred(parse_eval("test-defined-symbol"), get_int(500)));
}


void test_parse_eval_bool_symbols() {
    ASSERT(b_equal_pred(parse_eval("t"), get_sym("t")));
    ASSERT(b_equal_pred(parse_eval("f"), get_sym("f")));
}


void test_parse_eval_empty_list() {
    ASSERT(b_equal_pred(parse_eval("()"), LISP_EMPTY));
}


void test_parse_eval_non_function_list() {
    ASSERT(parse_eval("(not-a-function 1 2)") == NULL);
}


void test_parse_eval_function_app() {
    ASSERT(b_equal_pred(parse_eval("((lambda (x y) (+ x y)) 2 3)"),
			get_int(5)));
}


void test_parse_eval_improper_list() {
    // TODO: add this test when improper lists can be parsed
    /* ASSERT(parse_eval("(+ 1 . 2)") == NULL); */
}


void test_parse_eval_lambda_function() {
    parse_eval("(define test-eval-lambda (lambda (x) x))");
    ASSERT(b_equal_pred(parse_eval("(eval test-eval-lambda)"),
			parse_eval("test-eval-lambda")));
}


int main() {
    init_setup();
    test_parse_eval_positive_ints();
    test_parse_eval_negative_ints();
    test_parse_eval_quoted_symbols();
    test_parse_eval_undefined_symbols();
    test_parse_eval_defined_symbols();
    test_parse_eval_bool_symbols();
    test_parse_eval_empty_list();
    test_parse_eval_non_function_list();
    test_parse_eval_function_app();
    test_parse_eval_lambda_function();
    printf("\nAll tests PASSED.");
}
