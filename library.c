#include "library.h"
#include <stdio.h> // delete this later

/*
char *fullupper(char *string) {
	char *original;

	//original = string;

	for(original=string;*original;original++)
		*original = toupper(*original);
	return string;
}
*/

char *fullupper(char *string) {
	char *original;

	for(original=string;*original;original++) {
		*original = toupper(*original);
	}

	return string;
}

char *fulllower(char *string) {
	char *original;

	for(original=string;*original;original++) {
		*original = tolower(*original);
	}

	return string;
}

char *itoa(int number) {
	char *numstr;
	int tmp;
	int size;
	int y;

	size=1;
	tmp=number;

	do {
		tmp=tmp/10;
		if(tmp)
			size++;
	} while(tmp);

	numstr = malloc(size+1);
	tmp = number;

	for(y=0;size>y;y++) {
		numstr[size-y-1] = (48)+(tmp%10);
		tmp = tmp/10;
	}

	return numstr;
}
