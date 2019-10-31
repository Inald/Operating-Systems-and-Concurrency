#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

//Semaphores for sync, delaying consumer and counting number of jobs
sem_t sSync, sDelayProducer, sFreeElements;
int produced = 0, consumed = 0;
//Create head and tail pointers to pointers for the linked list
struct element *ptrH = NULL, *ptrT = NULL;
struct element **head = &ptrH, **tail = &ptrT;

void visualisation(int sender, pid_t ID)
{
    int count;
    struct element *elem;
    //If 0, sender is producer else consumer
    if(sender > 0)
    {
        printf("Producer %d Produced = %d Consumed = %d: ", ID, produced, consumed);
    }
    else
    {
        printf("Consumer %d Produced = %d Consumed = %d: ", ID, produced, consumed);
    }
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
    int count ; 
    pid_t cID = 1;
    sem_wait(&sDelayProducer);
    while(consumed < MAX_NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        sem_getvalue(&sFreeElements, &count);
        printf("sFreeElements = %d\n", count);
        removeFirst(head, tail);
        consumed++;
        visualisation(0, cID);
        if(count + 1 >= MAX_BUFFER_SIZE && consumed < MAX_NUMBER_OF_JOBS)
        {
            printf("AWAKEN PRODUCER\n");
            sem_post(&sDelayProducer);
        }
        sem_post(&sFreeElements);
        sem_post(&sSync);
    }
}


void * producerFunc()
{
    int count;
    pid_t pID = 1;
    sem_post(&sDelayProducer);
    while(produced < MAX_NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        sem_getvalue(&sFreeElements, &count);
        printf("sFreeElements = %d\n", count);
        addLast((char *)'*', head, tail);
        produced++;
        visualisation(1, pID);
        if(count - 1 <= 0 && produced < MAX_NUMBER_OF_JOBS)
        {
            printf("SLEEP PRODUCER\n");
            sem_wait(&sDelayProducer);
            printf("THIS SHOULD NOT PRINT   \n");
        }
        sem_wait(&sFreeElements);
        sem_post(&sSync);
    }
}

int main(int argc, char **argv)
{
    pthread_t consumer, producer;
    int finalSync, finalDelayProducer, finalElements;
    sem_init(&sSync, 0 , 1);
    sem_init(&sDelayProducer, 0 , 0);
    sem_init(&sFreeElements, 0, MAX_BUFFER_SIZE);
    pthread_create(&producer, NULL, producerFunc, NULL);
    pthread_create(&consumer, NULL, consumerFunc, NULL);
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    //Final values of semapores
    sem_getvalue(&sSync, &finalSync);
    sem_getvalue(&sDelayProducer,&finalDelayProducer);
    sem_getvalue(&sFreeElements, &finalElements);
    printf("sSync = %d, sDelayProducer = %d, sFreeElements = %d\n", finalSync, finalDelayProducer, finalElements);
    return 0;
}