#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "rwlock.h"

#define ITERATION 10
#define W 10 // reader thread count
#define R 30 //writer thread count

struct rwlock lock1;

void *reader (void *p)
{
	int i;

	for (i=0; i<ITERATION; ++i) {
		rw_reader_lock (&lock1);
		// read the data
		printf ("reading data \n");
		// usleep (random value) - maybe
		rw_reader_unlock (&lock1);

		// sleep for some microseconds
		usleep (5000); // can change to a random value
	}
	return 0;
//	pthread_exit (NULL);
}

void *writer (void *p)
{
	int i;

	for (i=0; i<ITERATION; ++i) {
		rw_writer_lock (&lock1);
		// write the data
		printf ("writing data \n");
		// usleep (random value) - maybe
		rw_writer_unlock (&lock1);

		// sleep for some microseconds
		usleep (5000); // can change to a random value
	}
	return 0;
//	pthread_exit (NULL);
}

int
main(int argc, char  *argv[])
{
	pthread_t rtid[R];
	pthread_t wtid[W];
	int i;

	rw_init (&lock1);
	// create reader and writer  threads
	for (i=0; i<W; ++i)
		pthread_create (&wtid[i], NULL, writer, NULL);
	for (i=0; i<R; ++i)
		pthread_create (&rtid[i], NULL, reader, NULL);


	// wait for threads to finish
	for (i=0; i<W; ++i)
		pthread_join (wtid[i], NULL);
	for (i=0; i<R; ++i)
		pthread_join (rtid[i], NULL);

	rw_destroy (&lock1);

	return 0;
}
