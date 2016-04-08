#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

int in_sum = 0;
int n;
int k;
void *pointFinder(void *param){
  // random number generator is created
  srand(time(NULL));
  int sum = 0;
  int i;
  for (i = 0; i < k; i++){

    // make up a number between -1 and 1
    double x = ((((double)(rand() % 1000000))/(1000000/2)) - 1);
    double y = ((((double)(rand() % 1000000))/(1000000/2)) - 1);

    //if their sum of suqares less than 1, that means it is in the circle
    if (x*x + y*y < 1)
      sum++;
  }

  // add the # of points in the circle to the sum in shared mem.
  in_sum += sum;
  return 0;
}

int main(int argc, char *argv[]){
  if (argc != 3){
    fprintf(stderr, "Wrong number of arguments!\n");
    exit(0);
  }

  n = atoi(argv[1]);
  k = atoi(argv[2]);

  if (n <= 0 || n >= 31){
    fprintf(stderr, "Please give an integer for the # of threads between 1 and 30\n");
    exit(0);
  }

  if (k <= 0){
    fprintf(stderr, "Please give an integer for the # of numbers to be made up more than 0\n");
    exit(0);
  }

  pthread_t tids[k]; /* id of the created thread */

  pthread_attr_t attr;  /* set of thread attributes */

  int i;
  pthread_attr_init (&attr);
  for(i = 0; i < n; i++){
    pthread_create (&(tids[i]), &attr, pointFinder, NULL);
  }

  for(i = 0; i < n; i++){
    pthread_join (tids[i], NULL);
  }

  // the formula is 4*<# of point in the circle>/(n*k)
  double dbl_sum = (double)(4*in_sum);
  double divider = (double)(n*k);
  double num = dbl_sum/divider;

  printf("%f", num);
  exit(0);
}
