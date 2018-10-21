// misc.h
// Header for miscellaneous utilities.


#ifndef MISC_H
#define MISC_H


#define FOUND_BUG found_bug(__FILE__, __LINE__, __func__);


void found_bug(const char * file, int line, const char * func);


#endif
