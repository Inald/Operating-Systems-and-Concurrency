#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

//Semaphores for sync, delaying consumer
//Counting semaphore represents number of free elements (number of jobs not filled)
sem_t sSync, sDelayProducer, sFreeElements;
int produced = 0, consumed = 0, producerAwake = 0;
//Create head and tail pointers to pointers for the linked list
struct element **priorityArray[MAX_PRIORITY];
struct element **head, **tail;


void visualisation(int sender, int ID)
{
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
    printf("\n");
}

void * consumerFunc(void *id)
{
    int count, cID = *((int*)id); 
    struct process * firstProcess;
    while(consumed < MAX_NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        sem_getvalue(&sFreeElements, &count);
        //If elements exist, can remove
        if(count < MAX_BUFFER_SIZE)
        {
            firstProcess = removeFirst(head, tail);
            consumed++;
            visualisation(0, cID);
            sem_post(&sFreeElements);
        }
        sem_post(&sSync);
        //Wake up producer
        if(consumed < MAX_NUMBER_OF_JOBS && producerAwake == 0)
        {
            producerAwake = 1;
            sem_post(&sDelayProducer);
        }
    }
}


void * producerFunc(void *id)
{
    int count, pID = (*(int *)id);
    sem_wait(&sDelayProducer);
    while(produced < NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        sem_getvalue(&sFreeElements, &count);
        //If free space exists in buffer, add to buffer
        if(count > 0)
        {
            addLast(generateProcess(), head, tail);
            produced++;
            visualisation(1, pID);
            sem_wait(&sFreeElements);
        }
        sem_post(&sSync);
        //Sleep producer
        if(produced < MAX_NUMBER_OF_JOBS && producerAwake == 1)
        {
            producerAwake = 0;
            sem_wait(&sDelayProducer);
        }
    }
}

int main(int argc, char **argv)
{
    pthread_t consumer, producer;
    int finalSync, finalDelayProducer, finalElements;
    for(int i = 0; i < MAX_PRIORITY; i++)
    {
        priorityArray[i] = NULL;
    }
    head = priorityArray[0];
    tail = priorityArray[MAX_PRIORITY - 1];
    sem_init(&sSync, 0 , 1);
    sem_init(&sDelayProducer, 0 , 1);
    sem_init(&sFreeElements, 0, MAX_BUFFER_SIZE);
    for(int i = 0; i < NUMBER_OF_PRODUCERS; i++)
    {
        pthread_create(&producer, NULL, producerFunc, (void *) &i);
    }
    //Add multiple consumeCount amounts of consumers, with associated ids
    for(int i = 0; i < NUMBER_OF_CONSUMERS; i++)
    {
        pthread_create(&consumer, NULL, consumerFunc, (void *)&i);
    }
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    //Final values of semapores
    sem_getvalue(&sSync, &finalSync);
    sem_getvalue(&sDelayProducer,&finalDelayProducer);
    sem_getvalue(&sFreeElements, &finalElements);
    printf("sSync = %d, sDelayProducer = %d, sFreeElements = %d\n", finalSync, finalDelayProducer, finalElements);
    return 0;
}
