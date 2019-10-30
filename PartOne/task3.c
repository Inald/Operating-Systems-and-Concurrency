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
int produced = 0, consumed = 0, counter = 0, errors = 0;
//Create head and tail pointers to pointers for the linked list
struct element *ptrH = NULL, *ptrT = NULL;
struct element **head = &ptrH, **tail = &ptrT;

void visualisation(int sender, pid_t ID)
{
    //Sender is >0 for a producer or 0 for a consumer
    struct element *elem;
    if(counter > MAX_BUFFER_SIZE)
    {
        printf("ERROR\n\n");
        errors++;
    }
    if(sender > 0)
    {
        printf("Producer %d, Produced = %d Consumed = %d: Count = %d ", ID, produced, consumed, counter);
    }
    else
    {
        printf("Consumer %d, Produced = %d Consumed = %d: Count = %d ", ID, produced, consumed, counter);
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
    int count;
    pid_t cID = 1;
    sem_post(&sDelayProducer);
    while(consumed < MAX_NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        if(counter > 0)
        {
            removeFirst(head, tail);
            consumed++;
            counter--;
            visualisation(0, cID);
        }
        else
        {
            sem_post(&sDelayProducer);
        }
        sem_post(&sSync);
    }
}


void * producerFunc()
{
    int count;
    pid_t pID = 1;
    while(produced < MAX_NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        addLast((char *)'*', head, tail);
        produced++;
        counter++;
        visualisation(1, pID);
        sem_post(&sSync);
        if(counter >= MAX_BUFFER_SIZE && produced < MAX_NUMBER_OF_JOBS)
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
    sem_init(&sDelayProducer, 0 , 0);
    sem_init(&sFull, 0, 0);
    pthread_create(&producer, NULL, producerFunc, NULL);
    pthread_create(&consumer, NULL, consumerFunc, NULL);
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    //Final values of semapores
    sem_getvalue(&sSync, &finalSync);
    sem_getvalue(&sDelayProducer,&finalDelayProducer);
    sem_getvalue(&sFull, &finalFull);
    printf("sSync = %d, sDelayProducer = %d, finalFull = %d, Errors: %d\n", finalSync, finalDelayProducer, finalFull, errors);
    return 0;
}