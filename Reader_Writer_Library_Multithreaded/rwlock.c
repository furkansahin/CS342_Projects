

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "rwlock.h"

void rw_init (struct rwlock *L)
{
	// Initialize the mutex variables  in the structure pointed by L.
	// Do some other initialization if your feel necessary.
//	pthread_mutex_init(&(L->mutex), NULL);
//	pthread_mutex_init(&(L->wrt), NULL);
	L->readcount = 0;
}

void rw_writer_lock (struct rwlock *L)
{
	pthread_mutex_lock(&(L->wrt));
}

void rw_writer_unlock (struct rwlock *L)
{
	pthread_mutex_unlock(&(L->wrt));
}

void rw_reader_lock (struct rwlock *L)
{
	pthread_mutex_lock(&(L->mutex));
	(L->readcount)++;
	if(L->readcount == 1){
		rw_writer_lock(L);
	}
	pthread_mutex_unlock(&(L->mutex));
}

void rw_reader_unlock (struct rwlock *L)
{
	pthread_mutex_lock(&(L->mutex));
	(L->readcount)--;
	if(L->readcount == 0){
		rw_writer_unlock(L);
	}
	pthread_mutex_unlock(&(L->mutex));
}

void rw_destroy (struct rwlock *L)
{
	// Destroy the mutex variables pointed by L.
	pthread_mutex_destroy(&(L->mutex));
	pthread_mutex_destroy(&(L->wrt));
//	free(L);
}
