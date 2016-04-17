#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

struct mapper{
    int size;
    struct mapper *next;
};

/* Your Library Global Variables */
void *segmentptr; // points to the start of memory segment to manage
int   segmentsize;  // size of segment to manage

//You can define more variables if you wish

// printfs are for debugging; remove them when you use/submit your library

int s_create (int size)
{
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

    // adding a struct to the head of the segment to describe it as a free segment
    ((struct mapper *) segmentptr) -> size = 0;
    ((struct mapper *) segmentptr) -> next = NULL;
    //////////////////////////////////////////////////////////////////////////////
    return (0);
}


void *s_alloc(int objectsize)
{
    if (objectsize < segmentsize){
        struct mapper * tempseg = (struct mapper *)segmentptr;
        while (tempseg -> next != NULL){
            if (objectsize + sizeof(struct mapper) <= (char *)(tempseg -> next) - tempseg -> size - (char *)tempseg - sizeof(struct mapper)){
                struct mapper *temp = tempseg -> next;
                tempseg -> next = (struct mapper *)((char *)tempseg + sizeof(struct mapper) + (tempseg -> size));
                tempseg -> next -> next = temp;
                tempseg -> next -> size = objectsize;
                return (tempseg -> next) + 1;
            }
            tempseg = tempseg -> next;
        }
        if (objectsize + sizeof(struct mapper) <= (char *)segmentptr + segmentsize - tempseg -> size - (char *)tempseg -  sizeof(struct mapper)){
            tempseg -> next =(struct mapper *)((char *)tempseg + sizeof(struct mapper) + (tempseg -> size));
            tempseg -> next -> next = NULL;
            tempseg -> next -> size = objectsize;
            return (tempseg -> next) + 1;
        }
    }
    return (NULL);		// if not success
}

void s_free(void *objectptr)
{
    struct mapper *tempseg = segmentptr;
    while (tempseg -> next && tempseg -> next != (struct mapper *)objectptr - 1){
        tempseg = tempseg->next;
    }
    if (tempseg -> next){
        struct mapper *temp = tempseg -> next -> next;
        tempseg -> next -> size = 0;
        tempseg -> next -> next = NULL;
        tempseg -> next = temp;
    }
    return;
}

void s_print(void)
{
    struct mapper *tempseg = segmentptr;
    printf("Alloc = from:%p\t to:%p\t size:%lu\t *****ALLOCATED FOR IMPLEMENTATION PURPOSES*****\n",
           tempseg,
           tempseg + 1,
           sizeof(struct mapper));
    if (!tempseg -> next && (segmentsize - sizeof(struct mapper) > 0)){
        printf ("Free = from:%p\t to:%p\t size:%lu\n", tempseg + 1
                , (char *)tempseg + segmentsize
                , segmentsize - sizeof(struct mapper));
    }
    else{
        while (tempseg){
            if (tempseg -> size != 0){
                printf("Alloc = from:%p\t to:%p\t size:%lu\n", tempseg
                        , (char *)tempseg + tempseg -> size
                        , tempseg -> size + sizeof(struct mapper));
            }
            if (tempseg -> next && ((char *)(tempseg -> next) - tempseg -> size - (char *)tempseg - sizeof(struct mapper) > 0)){
                printf("Free = from:%p\t to:%p\t size:%lu\n", tempseg -> size + (char *)tempseg + sizeof(struct mapper),
                       tempseg -> next,
                       (char *)(tempseg -> next) - tempseg -> size - (char *)tempseg - sizeof(struct mapper));
            }
            if ((!tempseg -> next) && ((char *)(segmentptr) + segmentsize - tempseg -> size - (char *)tempseg - sizeof(struct mapper) > 0)){
                printf("Free = from:%p\t to:%p\t size:%lu\n", tempseg -> size + (char *)tempseg + sizeof(struct mapper),
                       (char *)segmentptr + segmentsize,
                       (char *)(segmentptr) + segmentsize - tempseg -> size - (char *)tempseg - sizeof(struct mapper));
            }
            tempseg = tempseg -> next;
        }
    }
    return;
}
