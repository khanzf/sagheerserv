/*
	The purpose of this lib is for functions which
	are probably going to be used over and over again
	but which aren't in libc for one reason or another
*/

#ifndef _LIBRARY_H
#define _LIBRARY_H

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

char *fullupper(char *string);
char *fulllower(char *string);
char *itoa(int number); // does not do negatives :-)

#endif
