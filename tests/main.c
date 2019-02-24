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


int main() {
    init_setup();
    test_parse_eval_positive_ints();
    test_parse_eval_negative_ints();
    test_parse_eval_quoted_symbols();
    test_parse_eval_undefined_symbols();
    test_parse_eval_defined_symbols();
    test_parse_eval_bool_symbols();
    printf("\nAll tests PASSED.");
}
