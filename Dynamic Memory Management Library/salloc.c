#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

struct fields{
	unsigned long size;
	void *segment;
	struct fields *next;
}

/* Your Library Global Variables */
void *segmentptr; // points to the start of memory segment to manage
int   segmentsize;  // size of segment to manage
void *segments;

//You can define more variables if you wish
struct fields *field;

// printfs are for debugging; remove them when you use/submit your library

int s_create (int size)
{
	////////////// GIVEN CODE STARTS ////////////
	int i;
	void *endptr;
	char *cptr;
	void *vp;

	segmentptr = sbrk(0);     // end of data segment
	segmentsize = size;

	vp = sbrk(size); // extend data segment by indicated amount
	if (vp == ((void *)-1)) {
		printf ("segment creation failed\n");
		return (-1);
	}

	endptr = sbrk(0);

	printf("segmentstart=%lx, segmentend=%lx, segmentsize=%lu bytes\n",
	       (unsigned long)segmentptr,
	       (unsigned long)endptr, (unsigned long)(endptr - segmentptr));

	//test the segment
	printf("---starting testing segment\n");
	cptr = (char *)segmentptr;
	for (i = 0; i < size; ++i)
		cptr[i] = 0;
	printf("---segment test ended - success\n");
	////////// IMPLEMENTED CODE STARTS //////////
	float x = ((float)size) / 84;
	segments = segmentptr + (int)(20 * x);

	/////////////////////////////////////////////
	return (0);
}


void *s_alloc(int objectsize)
{

	printf("s_alloc called\n");

	return (NULL);		// if not success
}

void s_free(void *objectptr)
{

	printf("s_free called\n");

	return;
}

void s_print(void)
{
	printf("s_print called\n");
	return;
}
