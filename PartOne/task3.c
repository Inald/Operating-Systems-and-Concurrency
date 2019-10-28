#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

//Semaphores for sync, delaying consumer and counting number of jobs
sem_t sSync, sDelayProducer, sFull;
int produced = 0, consumed = 0;
//Create head and tail pointers to pointers for the linked list
struct element *ptrH = NULL, *ptrT = NULL;
struct element **head = &ptrH, **tail = &ptrT;

void visualisation()
{
    struct element *elem;
    int count;
    sem_getvalue(&sFull, &count);
    printf("Produced = %d Consumed = %d: Count = %d ", produced, consumed, count);
    //Print out element data of each element in the list
    if(*head)
    {
        for(struct element *elem = *head; elem != NULL; elem = elem ->pNext)
        {
            printf("%c", elem -> pData);
        }
    }
    printf("\n");
}

void * consumerFunc()
{
    int count;
    sem_wait(&sDelayProducer);
    while(consumed < MAX_NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        sem_getvalue(&sFull, &count);
        //If list is empty and max jobs isn't exceeded, put to sleep
        if(count <= 0 && consumed != MAX_NUMBER_OF_JOBS)
        {
            sem_post(&sDelayProducer);
        }
        else
        {
            //Consumes item at head of list
            removeFirst(head, tail);
            consumed++;
            //Get current no. of items in list, store in count
            sem_wait(&sFull);
            //Print list data
            visualisation();
        }
        sem_post(&sSync);
    }
}


void * producerFunc()
{
    int count;
    while(produced < MAX_NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        //Adds element to the list with * character as data
        addLast((char *)'*', head, tail);
        produced++;
        sem_post(&sFull);
        sem_getvalue(&sFull, &count);
        visualisation();
        sem_post(&sSync);
        //If list is not empty and max jobs not exceeded, wake up consumer
        if(count >= MAX_BUFFER_SIZE)
        {
            sem_wait(&sDelayProducer);
        }
    }
}

int main(int argc, char **argv)
{
    pthread_t consumer, producer;
    int finalSync, finalDelayProducer, finalFull;
    sem_init(&sSync, 0 , 1);
    sem_init(&sDelayProducer, 0 , 1);
    sem_init(&sFull, 0, 0);
    pthread_create(&producer, NULL, producerFunc, NULL);
    pthread_create(&consumer, NULL, consumerFunc, NULL);
    pthread_join(consumer, NULL);
    pthread_join(producer, NULL);
    //Final values of semapores
    sem_getvalue(&sSync, &finalSync);
    sem_getvalue(&sDelayProducer,&finalDelayProducer);
    sem_getvalue(&sFull, &finalFull);
    printf("sSync = %d, sDelayProducer = %d, finalFull = %d\n", finalSync, finalDelayProducer, finalFull);
    return 0;
}