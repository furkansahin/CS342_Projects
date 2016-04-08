#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>

#define MAX_SIZE 1024
#define MAX_CHILD 26

// created to store the page numbers for each word node
struct int_lst {
  int num;
  struct int_lst *next;
};

// each word node
struct node
{
  struct int_lst *nums;
  char *word;
  struct node *next;
};

struct node *nodes[MAX_CHILD];

// printing method to file
void print(struct node *head, FILE **file){
  struct node *ptr = head;

  // continues till the end of the linkedlist
  while (ptr != NULL){
    fprintf(*file, "%s ", ptr -> word);
    struct int_lst *num_ptr = ptr -> nums;
    fprintf(*file, "%d", num_ptr -> num);

    num_ptr = num_ptr -> next;

    // prints the line numbers
    while (num_ptr != NULL){
      fprintf(*file, ", %d", num_ptr -> num);
      num_ptr = num_ptr -> next;
    }
    // next line
    fprintf(*file, "\n");
    ptr = ptr -> next;
  }
}

// deletes the line number linkedlist
void delete_nums(struct int_lst *head){
  if (head){
    delete_nums(head -> next);
    free(head);
  }
}

//deletes the whole list
void delete_lst(struct node *head){
  if (head){
    delete_lst(head -> next);
    free(head -> word);
    delete_nums(head -> nums);
    free(head);
  }
}

// adds the line number node to the specified node's line list
void addNum(int line, struct int_lst **head, char sorted){
  struct int_lst *newNode = (struct int_lst *) malloc(sizeof(struct int_lst));
  newNode -> num = line;
  newNode -> next = NULL;

  if (*head == NULL){
    *head = newNode;
  }
  else{
    // If it does not necessary to be sorted, adds it to the head directly
    if (line < (*head) -> num || sorted == 0){
      if (sorted == 0 && line == (*head) -> num){
        free(newNode);
      }
      else{
        newNode -> next = *head;
        *head = newNode;
      }
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

// creates word node and adds it to the appropriate location
void add(char *word, int line, struct node **head, char sorted)
{
  struct node *newNode = (struct node*)malloc(sizeof(struct node));
  newNode->word = (char*)malloc((strlen(word)+1)*sizeof(char));
  strcpy(newNode->word,word);
  newNode->nums = NULL;
  newNode->next = NULL;

  if (*head == NULL){
    *head = newNode;
    addNum(line, &(newNode -> nums), sorted);
  }
  else{
    if (strcmp(word, (*head) -> word) < 0 || sorted == 0){
      newNode -> next = *head;
      *head = newNode;
      addNum(line, &(newNode -> nums), sorted);
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
        addNum(line, &(newNode -> nums), sorted);
      }
      else {
        if (strcmp(word, traveller -> next -> word) < 0){
          newNode -> next = traveller -> next;
          traveller -> next = newNode;
          addNum(line, &(newNode -> nums), sorted);
        }
        else if (strcmp(word, traveller -> next -> word) == 0){
          free (newNode -> word);
          free (newNode);
          addNum(line, &(traveller -> next -> nums), sorted);
        }
      }
    }
    else{
      free (newNode -> word);
      free (newNode);
      addNum(line, &((*head) -> nums), sorted);
    }
  }
}

// thread function
void *threadFun(void *param){
  int id = *((int *) param);

  struct node *head = (nodes[id]);
  struct node *sorted = NULL;
  while (head != NULL){
    struct int_lst *tmp = head -> nums;
    while (tmp != NULL && tmp -> num){
      add(head -> word, tmp -> num, &sorted, 1);
      tmp = tmp -> next;
    }
    head = head -> next;
  }
  delete_lst(nodes[id]);
  nodes[id] = sorted;
  free(param);
  return 0;
}

int main(int argc, char *argv[]){
  if (argc != 4){
    fprintf(stderr, "Wrong number of arguments!\n");
    exit(0);
  }

  int k = atoi(argv[1]);
  if (k <= 0 || k >= 27){
    fprintf(stderr, "Please give an integer for the # of threads between 1 and 26\n");
    exit(0);
  }

  int i;

  // reading input file and store it in linkedlist
  FILE *input = fopen(argv[2],"r");
  char line[4096];
  char *word;
  int line_c = 1;
  while( fgets(line, 4096, input) != NULL)
  {
    word = strtok(line, " \n\r");
    while(word){
      int counter;
      for (counter = 0; counter <= strlen(word); counter++){
        word[counter] = tolower(word[counter]);
      }
      int index = (word[0] - 'a');
      int worker = 26/k;

      worker = index/worker;
      if (worker != 0 && worker > k-1){
        worker = k-1;
      }
      add(word, line_c, &(nodes[worker]), 0);
      word = strtok(NULL, " \n\r");
    }
    line_c++;
  }
  fclose(input);

  pthread_t tids[k]; /* id of the created thread */

  pthread_attr_t attr;  /* set of thread attributes */

  pthread_attr_init (&attr);

  int *arg;
  for(i = 0; i < k; i++){
    arg = malloc(sizeof(*arg));
    *arg = i;
    pthread_create (&(tids[i]), &attr, threadFun, (void *)arg);
  }

  for(i = 0; i < k; i++){
    pthread_join (tids[i], NULL);
  }

  FILE *file = fopen(argv[3], "w+");

  // printing all of the lists in one file
  for (i = 0; i < k; i++){
    print(nodes[i], &file);
  }
  for (i = 0; i < k; i++){
    delete_lst(nodes[i]);
  }
  fclose(file);
  exit(0);
}
