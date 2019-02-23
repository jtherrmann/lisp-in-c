#include <stdio.h>

#include "builtins.h"
#include "obj.h"
#include "error.h"
#include "parse-eval.h"
#include "setup.h"


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


int main() {
    init_setup();
    test_parse_eval_positive_ints();
    test_parse_eval_negative_ints();
    printf("All tests passed.");
}
