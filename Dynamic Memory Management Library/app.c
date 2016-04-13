#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "salloc.h"

int main(int argc, char *argv[])
{
	int ret;
	int size;
	void *x1, *x2, *x3;	// object pointers

	if (argc != 2) {
		printf("usage: app <size in KB>\n");
		exit(1);
	}

	size = atoi(argv[1]);

	ret =  s_create (size * 1024);
	if (ret == -1) {
		printf ("could not create segment\n");
		exit (1);
	}

	x1 = s_alloc(600);
	x2 = s_alloc(4500);
	x3 = s_alloc(1300);

	s_free(x1);
	s_free(x2);
	s_free(x3);


	return 0;
}
