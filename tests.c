// tests.c
// Source for tests.


#include <stdio.h>

#include "obj.h"
#include "parse.h"
#include "tests.h"


void run_tests() {
    printf("Running tests.\n");

    int total = 0;

    total += test_obj();
    total += test_parse();

    printf("Ran %d tests.\n", total);
    printf("All tests pass.\n\n");
}

