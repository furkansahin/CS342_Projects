

struct rwlock {
	// put your mutex variable definitions here.
	// you can put some other variables.
	pthread_mutex_t wrt;
	pthread_mutex_t mutex;
	int readcount;
};


void rw_init (struct rwlock *L);
void rw_writer_lock (struct rwlock *L);
void rw_writer_unlock (struct rwlock *L);
void rw_reader_lock (struct rwlock *L);
void rw_reader_unlock (struct rwlock *L);
void rw_destroy (struct rwlock *L);
