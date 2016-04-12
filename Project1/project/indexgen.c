#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#define MQNAME "/message_queue%d"
#define MAX_SIZE 1024
struct int_lst {
  int num;
  struct int_lst *next;
};

struct node
{
  struct int_lst *nums;
  char *word;
  struct node *next;
};
void addNum(int line, struct int_lst **head){
  struct int_lst *newNode = (struct int_lst *) malloc(sizeof(struct int_lst));
  newNode -> num = line;
  newNode -> next = NULL;

  if (*head == NULL){
    *head = newNode;
  }
  else{
    if (line < (*head) -> num){
      newNode -> next = *head;
      *head = newNode;
    }
    else if (line == (*head) -> num){
      free(newNode);
    }
    else{
      struct int_lst *traveller = (*head);
      while (traveller -> next != NULL && line > traveller -> next -> num){
        traveller = traveller -> next;
      }
      if (traveller -> next == NULL){
        traveller -> next = newNode;
        newNode -> next = NULL;
      }
      else {
        if (line < traveller -> next -> num){
          newNode -> next = traveller -> next;
          traveller -> next = newNode;
        }
        else if (line == traveller -> next -> num){
          free(newNode);
        }
      }
    }
  }
}
void add(char *word, int line, struct node **head)
{
  struct node *newNode = (struct node*)malloc(sizeof(struct node));
  newNode->word = (char*)malloc((strlen(word)+1)*sizeof(char));
  strcpy(newNode->word,word);
  newNode->nums = NULL;
  if (*head == NULL){
    *head = newNode;
    addNum(line, &(newNode -> nums));
  }
  else{
    if (strcmp(word, (*head) -> word) < 0){
      newNode -> next = *head;
      *head = newNode;
      addNum(line, &(newNode -> nums));
    }
    else if (strcmp(word, (*head) -> word) > 0)
    {
      struct node *traveller = (*head);
      while (traveller -> next != NULL && strcmp(word, traveller -> next -> word) > 0){
        traveller = traveller -> next;
      }
      if (traveller -> next == NULL){
        traveller -> next = newNode;
        newNode -> next = NULL;
        addNum(line, &(newNode -> nums));
      }
      else {
        if (strcmp(word, traveller -> next -> word) < 0){
          newNode -> next = traveller -> next;
          traveller -> next = newNode;
          addNum(line, &(newNode -> nums));
        }
        else if (strcmp(word, traveller -> next -> word) == 0){
          free (newNode -> word);
          free (newNode);
          addNum(line, &(traveller -> next -> nums));
        }
      }
    }
    else{
      free (newNode -> word);
      free (newNode);
      addNum(line, &((*head) -> nums));
    }
  }
}
void delete_nums(struct int_lst *head){
  if (head){
    delete_nums(head -> next);
    free(head);
  }
}
void delete_lst(struct node *head){
  if (head){
    delete_lst(head -> next);
    free(head -> word);
    delete_nums(head -> nums);
    free(head);
  }
}

void print(struct node *head, char *file){
  FILE *out = fopen(file, "w+");
  struct node *ptr = head;

  while (ptr != NULL){
    fprintf(out, "%s ", ptr -> word);
    struct int_lst *num_ptr = ptr -> nums;
    fprintf(out, "%d", num_ptr -> num);
    num_ptr = num_ptr -> next;
    while (num_ptr != NULL){
      fprintf(out, ", %d", num_ptr -> num);
      num_ptr = num_ptr -> next;
    }
    fprintf(out, "\n");
    ptr = ptr -> next;
  }
  fclose(out);
}

struct item {
  int line;
  char word[64];
};

int main(int argc, char *argv[]){
  if (argc != 4){
    fprintf(stderr, "Wrong number of arguments!\n");
    exit(0);
  }

  int k = atoi(argv[1]);
  if (k <= 0 || k >= 6){
    fprintf(stderr, "Please give an integer for the # of processes between 1 and 5\n");
    exit(0);
  }
  mqd_t mq_s[k];
  int numChild = k;
  int i;
  for (i = 0; i < k; i++){
    // max name length for message_queue is 32
    char buf[32];
    char *name = MQNAME;
    sprintf(buf, name, i);
    mq_s[i] = mq_open(buf, O_RDWR | O_CREAT, 0666, NULL);
    mq_close(mq_s[i]);
    mq_s[i] = mq_open(buf, O_RDWR | O_CREAT, 0666, NULL);
    if (mq_s[i] == -1){
      printf("Queue cannot be created. Error: %s *** %d\n", strerror(errno), errno);
      exit(1);
    }
  }
  for(i = 0; i < k ; i++){
    int child = fork();
    if (child == 0){
      struct mq_attr mq_attr;
      mq_getattr(mq_s[i], &mq_attr);
      struct node *head = NULL;
      char *in = (char *) malloc(sizeof(struct item));
      while (1){
        if (mq_receive(mq_s[i], (char *) in, mq_attr.mq_msgsize, NULL) >= 0){
          struct item *_in = (struct item *)in;
          if (_in -> line == -1){
            ///WRITE TO FILE!!
            char out[16];
            strcpy(out, argv[3]);
            strcat(out, "%d");
            sprintf(out, out, i);
            print(head, out);
            delete_lst(head);
            mq_close(mq_s[i]);
            free(in);
            exit(0);
          }
          add(_in -> word, _in -> line, &head);
        }
        else{
          printf("RECEIVE FAILED FOR QUEUE - %d\n", i);
        }
      }
      free(in);
    }
  }

  /// START TAKING INPUTS!!!
  FILE *input = fopen(argv[2],"r");
  struct item item;
  // max line length is decided as 8192 characters including the null pointer.
  char line[4096];
  char *word;
  int line_c = 1;
  while( fgets(line, 4096, input) != NULL)
  {
    word = strtok(line, " \n");
    while(word){
      int counter;
      for (counter = 0; counter <= strlen(word); counter++){
        word[counter] = tolower(word[counter]);
      }
      item.line = line_c;
      memset(item.word, '\0', 64);
      strcpy(item.word, word);
      int index = (item.word[0] - 'a');
      int worker = 26/numChild;

      worker = index/worker;
      if (worker != 0 && worker > numChild-1){
        worker = numChild-1;
      }
      int m = mq_send(mq_s[worker], (char *) &item, sizeof(item), 0);
      if(m == -1){

        printf("COULD NOT SENT\t%d\t%d\n", worker, index);
        exit(1);
      }
      word = strtok(NULL, " \n");
    }
    line_c++;
  }
  fclose(input);

  for(i = 0; i < k; i++){
    struct item item;
    item.line = -1;
    mq_send(mq_s[i], (char *) &item, sizeof(item), 0);
  }

  for(i = 0; i < k; i++){
    wait(NULL);
  }

  FILE *file = fopen(argv[3], "w+");
  for (i = 0; i < k; i++){
    char out[16];
    strcpy(out, argv[3]);
    strcat(out, "%d");
    sprintf(out, out, i);
    FILE *file_temp = fopen(out, "r");

    char line[8192];
    while (fgets(line, 8192, file_temp) != NULL){
      fprintf(file, "%s", line);
    }
    fclose(file_temp);
    remove(out);
  }
  fclose(file);


  exit(0);
}
