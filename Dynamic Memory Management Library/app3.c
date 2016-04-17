#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "salloc.h"
#include <time.h>


int main(int argc, char *argv[])
{
	clock_t begin, end;
    double time_spent1, time_spent2, time_spent3, time_spent4;

	int ret;
	int size;
	void *x[1000];	// object pointers

	if (argc != 2) {
		printf("usage: app3 <size in KB>\n");
		exit(1);
	}

	size = atoi(argv[1]);

	ret =  s_create (size * 1024);
	if (ret == -1) {
		printf ("could not create segment\n");
		exit (1);
	}
    int i;
    for (i = 0; i < 1000; i++){
        if (i%2 == 0){
            x[i] = s_alloc(100);
        }
        else{
            x[i] = s_alloc(i * 50);
        }
    }

    s_print();

    for (i = 1; i < 1000; i += 2){
        s_free(x[i]);
    }

    begin = clock();

    for (i = 1; i < 1000; i += 2){
        x[i] = s_alloc(i * 50);
    }

    end = clock();
    time_spent1 = (double)(end - begin)/CLOCKS_PER_SEC;

    for (i = 1; i < 1000; i += 2){
        s_free(x[i]);
    }

    begin = clock();

    for (i = 1; i < 1000; i += 2){
        x[i] = s_alloc((1000 - i) * 50);
    }

    end = clock();
    time_spent2 = (double)(end - begin)/CLOCKS_PER_SEC;

    begin = clock();
    for (i = 0; i < 1000; i ++){
        s_free(x[i]);
    }
    end = clock();
    time_spent3 = (double)(end - begin)/CLOCKS_PER_SEC;

    for (i = 0; i < 1000; i++){
        if (i%2 == 0){
            x[i] = s_alloc(100);
        }
        else{
            x[i] = s_alloc(i * 50);
        }
    }

    begin = clock();
    for (i = 999; i >= 0; i--){
        s_free(x[i]);
    }
    end = clock();
    time_spent4 = (double)(end - begin)/CLOCKS_PER_SEC;

    printf ("t1:%f\n", time_spent1);
    printf ("t2:%f\n", time_spent2);
    printf ("t3:%f\n", time_spent3);
    printf ("t4:%f\n", time_spent4);


	return 0;
}
