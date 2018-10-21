// misc.c
// Source for miscellaneous utilities.


#include <stdio.h>
#include <stdlib.h>


void found_bug(const char * file, int line, const char * func) {
    // TODO: instructions for reporting
    printf("\n%s, line %d, in %s:\nIt looks like you have found a bug!\n\n",
	   file, line, func);
    exit(1);
}
