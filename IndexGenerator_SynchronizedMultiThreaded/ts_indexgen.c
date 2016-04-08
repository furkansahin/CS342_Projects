#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>

#define MAX_CHILD 8
#define MAX_BB 1000



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

// each item to be put in Bounded Buffer
struct item {
  int line;
  char word[64];
};

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


struct node *nodes[MAX_CHILD];
struct item buffer[MAX_CHILD][MAX_BB];
pthread_mutex_t mutex[MAX_CHILD];
pthread_mutex_t fullMutex[MAX_CHILD];
pthread_mutex_t emptyMutex[MAX_CHILD];
pthread_cond_t full[MAX_CHILD];
pthread_cond_t empty[MAX_CHILD];
int in[MAX_CHILD];
int out[MAX_CHILD];
int sizes[MAX_CHILD];
int buff_size;

// thread function
void *threadFun(void *param){
  int id = *((int *) param);

  struct node *sorted = NULL;
  while (1){
    // Wait if the buffer is empty

    while (sizes[id] == 0){
      pthread_cond_wait(&empty[id], &emptyMutex[id]);
    }
    // there is an element and I want to read it, so first lock and read!
    pthread_mutex_lock(&mutex[id]);
    if (buffer[id][out[id]].line != -1){
      add(buffer[id][out[id]].word, buffer[id][out[id]].line, &sorted, 1);
      sizes[id]--;
      out[id] = (out[id] + 1) % buff_size;
    }
    else{
      // locks are cancelled and the thread is terminated!
      pthread_mutex_unlock(&mutex[id]);
      pthread_cond_signal(&full[id]);
      break;
    }

    pthread_mutex_unlock(&mutex[id]);
    pthread_cond_signal(&full[id]);

  }
  nodes[id] = sorted;
  free(param);
  return 0;
}

int main(int argc, char *argv[]){
  clock_t begin, end;
  double time_spent;

  if (argc != 5){
    fprintf(stderr, "Wrong number of arguments!\n");
    exit(0);
  }

  int n = atoi(argv[1]);
  if (n <= 0 || n >= 9){
    fprintf(stderr, "Please give an integer for the # of threads between 1 and 8\n");
    exit(0);
  }


  buff_size = atoi(argv[2]);
  if (buff_size <= 0 || buff_size >= 1001){
    fprintf(stderr, "Please give an integer for the size of buffer between 1 and 1000\n");
    exit(0);
  }

  begin = clock();


  pthread_t tids[n]; /* id of the created thread */

  pthread_attr_t attr;  /* set of thread attributes */

  pthread_attr_init (&attr);

  int *arg;
  int i;
  for(i = 0; i < n; i++){
    arg = malloc(sizeof(*arg));
    *arg = i;
    sizes[i] = 0;
    pthread_mutex_init(&mutex[i], NULL);
    pthread_mutex_init(&fullMutex[i], NULL);
    pthread_mutex_init(&emptyMutex[i], NULL);
    pthread_cond_init(&full[i], NULL);
    pthread_cond_init(&empty[i], NULL);
    pthread_create (&(tids[i]), &attr, threadFun, (void *)arg);
  }

  // reading input file and store it in linkedlist

  FILE *input = fopen(argv[3],"r");
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
      int worker = 26/n;

      worker = index/worker;
      if (worker > n-1){
        worker = n-1;
      }
      // prepare the item to be put!
      struct item tmp;
      strcpy(tmp.word, word);
      tmp.line = line_c;

      //adding the word to the appropriate buffer
      while (sizes[worker] == buff_size){
        pthread_cond_wait(&full[worker], &fullMutex[worker]);
      }
      pthread_mutex_lock(&mutex[worker]);

      buffer[worker][in[worker]] = tmp;
      sizes[worker]++;
      in[worker] = (in[worker] + 1) % buff_size;
      pthread_mutex_unlock(&mutex[worker]);
      pthread_cond_signal(&empty[worker]);

      word = strtok(NULL, " \n\r");
    }
    line_c++;
  }
  fclose(input);

  for(i = 0; i < n;i++){
    struct item temp;
    temp.line = -1;
    while (sizes[i] == buff_size){
      pthread_cond_wait(&full[i], &fullMutex[i]);
    }
    pthread_mutex_lock(&mutex[i]);
    buffer[i][in[i]] = temp;
    sizes[i]++;
    in[i] = (in[i] + 1) % buff_size;
    pthread_mutex_unlock(&mutex[i]);
    pthread_cond_signal(&empty[i]);
  }


  for(i = 0; i < n; i++){
    pthread_join (tids[i], NULL);
  }

  FILE *file = fopen(argv[4], "w+");

  // printing all of the lists in one file
  for (i = 0; i < n; i++){
    print(nodes[i], &file);
  }
  for (i = 0; i < n; i++){
    delete_lst(nodes[i]);
  }
  fclose(file);
  end = clock();
  time_spent = (double)(end - begin)/CLOCKS_PER_SEC;
  printf("%f\n", time_spent );
  exit(0);
}
