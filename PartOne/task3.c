#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

sem_t sSync, sDelayConsumer, sCount;
int produced = 0, consumed = 0, sharedCounter = 0;
//Create head and tail pointers to pointers for the linked list
struct element *ptrH = NULL, *ptrT = NULL;
struct element **head = &ptrH, **tail = &ptrT;
/*displays the exact number of elements currently in buffer
everytime an element is added to or removed from the buffer*/
void visualisation()
{
    int i, count;
    sem_getvalue(&sCount, &count);
    printf("Produced = %d Consumed = %d ", produced, consumed);
    for(int i = 0; i < count; i++)
    {
        printf("*");
    }
    printf("\n");
}

void * consumerFunc(){
    int i = 0, temp = 0, count;
    sem_wait(&sDelayConsumer);
    while(i < MAX_BUFFER_SIZE){
        sem_wait(&sSync);
        i++;
        removeFirst(head, tail);
        consumed++;
        temp = consumed;
        sem_wait(&sCount);
        sem_getvalue(&sCount, &count);
        visualisation();
        sem_post(&sSync);
        if(count == 0 && produced == consumed && i != MAX_BUFFER_SIZE){
            sem_wait(&sDelayConsumer);
        }
    }
}


void * producerFunc(){
    int i = 0, count;
    while(i < MAX_BUFFER_SIZE){
        sem_wait(&sSync);
        i++;
        addLast((char *)'*', head, tail);
        produced++;
        sem_post(&sCount);
        visualisation();
        sem_getvalue(&sCount, &count);
        if(count == 1){
            sem_post(&sDelayConsumer);
        }
        sem_post(&sSync);
    }
}

int main(int argc, char **argv){
    pthread_t consumer, producer;
    int finalSync, finalDelayConsumer, finalCount;
    sem_init(&sSync, 0 , 1);
    sem_init(&sDelayConsumer, 0 ,0);
    pthread_create(&producer, NULL, producerFunc, NULL);
    pthread_create(&consumer, NULL, consumerFunc, NULL);
    pthread_join(consumer, NULL);
    pthread_join(producer, NULL);
    sem_getvalue(&sSync, &finalSync);
    sem_getvalue(&sDelayConsumer,&finalDelayConsumer);
    sem_getvalue(&sCount, &finalCount);
    printf("sSync = %d, sDelayConsumer = %d sCount = %d\n", finalSync, finalDelayConsumer, finalCount);
    return 0;
}