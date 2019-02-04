// hash.c
// Source for hash functions.


#include "hash.h"


// ============================================================================
// Public functions
// ============================================================================

unsigned hash_string(char * s) {
    // Source: K&R C, 2nd ed., p. 144.
    unsigned hashval = 0;
    for(long i = 0; s[i] != '\0'; ++i)
	hashval = s[i] + 31 * hashval;
    return hashval;
}
