#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <sys/ipc.h>
#include <sys/msg.h>
int main(){
  int children[4];
  int i;
  for(i = 0; i < 4 ; i++){
    children[i] = fork();
    if (children[i] == 0){
      printf("%d\n", i);
      break;
    }
  }
  for (i = 0; i < 4; i++){
    wait(NULL);
  }
  for (i = 0; i < 4; i++){
    if (children[i] == 0){
//      printf("child %d = 0\n", i);
    }
    else{
  //    printf ("parent %d != 0\n", i);
    }
  }


}
