#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

sem_t sSync, sDelayConsumer, sharedCounter;
int produced = 0, consumed = 0, count;
//Create head and tail pointers to pointers for the linked list
struct element *ptrH = NULL, *ptrT = NULL;
struct element **head = &ptrH, **tail = &ptrT;
/*displays the exact number of elements currently in buffer
everytime an element is added to or removed from the buffer*/
void visualisation()
{
    int counter;
    printf("Produced = %d  Consumed = %d  ", produced, consumed);
    for(int i = 0; i < sem_getvalue(&sharedCounter, &counter); i++)
    {
        printf("*");
    }
    printf("\n");
}

void * consumerFunc(){
    int i = 0, temp = 0;
    sem_wait(&sDelayConsumer);
    while(i < MAX_BUFFER_SIZE){
        sem_wait(&sSync);
        i++;
        removeFirst(head, tail);
        consumed++;
        sem_wait(&sharedCounter);
        temp = consumed;
        visualisation();
        sem_post(&sSync);
        if(temp == 0 && i != MAX_BUFFER_SIZE){
            sem_wait(&sDelayConsumer);
        }
    }
}


void * producerFunc(){
    int i = 0, counter;
    while(i < MAX_BUFFER_SIZE){
        sem_wait(&sSync);
        i++;
        addLast((char *)'*', head, tail);
        produced++;
        sem_post(&sharedCounter);
        visualisation();
        if(sem_getvalue(&sharedCounter, &counter) == 1){
            sem_post(&sDelayConsumer);
        }
        sem_post(&sSync);
    }
}

int main(int argc, char **argv){
    pthread_t consumer, producer;
    int finalSync, finalDelayConsumer;
    sem_init(&sSync, 0 , 1);
    sem_init(&sDelayConsumer, 0 , 0);
    pthread_create(&producer, NULL, producerFunc, NULL);
    pthread_create(&consumer, NULL, consumerFunc, NULL);
    pthread_join(consumer, NULL);
    pthread_join(producer, NULL);
    sem_getvalue(&sSync, &finalSync);
    sem_getvalue(&sDelayConsumer,&finalDelayConsumer);
    printf("sSync = %d, sDelayConsumer = %d\n", finalSync, finalDelayConsumer);
    return 0;
}