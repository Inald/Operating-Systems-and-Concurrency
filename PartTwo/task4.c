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
    struct process *firstProcess;
    struct timeval start, end;
    while(consumed < NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        sem_getvalue(&sFreeElements, &count);
        printf("Test 1\n");
        //If elements exist, can remove
        if(count < MAX_BUFFER_SIZE)
        {
            for(int i = 0; i < MAX_PRIORITY; i++)
            {
                printf("Test 2\n");
                firstProcess = ((struct process *)*priorityArray[i]);
                printf("Test 3\n");
                start = firstProcess -> oTimeCreated;
                end = firstProcess -> oMostRecentTime;
                printf("Test 6\n");
                consumed++;
            }
            visualisation(0, cID);
            sem_post(&sFreeElements);
        }
        sem_post(&sSync);
        //Wake up producer
        if(consumed < NUMBER_OF_JOBS && producerAwake == 0)
        {
            producerAwake = 1;
            sem_post(&sDelayProducer);
        }
    }
}


void * producerFunc(void *id)
{
    int count, pID = (*(int *)id);
    struct process *newProcess;
    sem_wait(&sDelayProducer);
    while(produced < NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        sem_getvalue(&sFreeElements, &count);
        printf("Test 4\n");
        //If free space exists in buffer, add to buffer
        if(count > 0)
        {
            newProcess = generateProcess();
            printf("Test 5\n");
            for(int i = 0; i < MAX_PRIORITY; i++)
            {
                if(newProcess -> iPriority == i)
                {
                    printf("Test 6\n");
                    addLast(newProcess, priorityArray[i], priorityArray[i]);
                    printf("Test 7\n");
                    produced++;
                }
            }
            visualisation(1, pID);
            sem_wait(&sFreeElements);
        }
        sem_post(&sSync);
        //Sleep producer
        if(produced < NUMBER_OF_JOBS && producerAwake == 1)
        {
            producerAwake = 0;
            sem_wait(&sDelayProducer);
        }
    }
}

int main(int argc, char **argv)
{
    pthread_t consumer, producer;
    int finalSync, finalDelayProducer, finalElements, id;
    for(int i = 0; i < MAX_PRIORITY; i++)
    {
        priorityArray[i] = malloc(sizeof(struct process *) * MAX_BUFFER_SIZE);
    }
    head = priorityArray[0];
    tail = priorityArray[MAX_PRIORITY - 1];
    sem_init(&sSync, 0 , 1);
    sem_init(&sDelayProducer, 0 , 1);
    sem_init(&sFreeElements, 0, MAX_BUFFER_SIZE);
    //Create producer and consumers
    for(int i = 0; i < NUMBER_OF_PRODUCERS; i++)
    {
        pthread_create(&producer, NULL, producerFunc, (void *)&i);
    }
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
