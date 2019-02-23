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
#include "parse-eval.h"
#include "print.h"
#include "setup.h"
#include "stack.h"


int main() {
    init_setup();

    printf("Welcome to Lisp!\n");
    printf("Exit with Ctrl-c\n\n");

    LispObject * result;
    while (true) {
	input = readline("> ");
	add_history(input);
	result = parse_eval(input);
	if (result != NULL) {
	    print_obj(result);
	    printf("\n");
	}
	free(input);
    }
    FOUND_BUG;
}
