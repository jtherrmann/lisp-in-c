#include <stdio.h>

#include "builtins.h"
#include "obj.h"
#include "error.h"
#include "parse-eval.h"
#include "setup.h"


// TODO: implement proper test assertion functions/macros
int main() {
    init_setup();
    ASSERT(b_equal_pred(parse_eval("123"), get_int(123)));
}
