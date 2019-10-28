#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

sem_t sSync, sDelayConsumer;
int produced = 0, consumed = 0, current = 0;
//Create head and tail pointers to pointers for the linked list
struct element *ptrH = NULL, *ptrT = NULL;
struct element **head = &ptrH, **tail = &ptrT;

void visualisation()
{
    struct element *elem;
    printf("Produced = %d Consumed = %d: ", produced, consumed);
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
    int temp = 0, count = 0;
    sem_wait(&sDelayConsumer);
    while(consumed < MAX_NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        removeFirst(head, tail);
        consumed++;
        current--;
        temp = produced - consumed;
        visualisation();
        sem_post(&sSync);
        if(temp == 0 && consumed != MAX_NUMBER_OF_JOBS)
        {
            sem_wait(&sDelayConsumer);
        }
    }
}


void * producerFunc()
{
    int count, temp;
    while(produced < MAX_NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        if(current < MAX_BUFFER_SIZE)
        {
            addLast((char *)'*', head, tail);
            produced++;
            current++;
            temp = produced - consumed;
            visualisation();
            if(temp == 1)
            {
                sem_post(&sDelayConsumer);
            }
        }
        sem_post(&sSync);
    }
}

int main(int argc, char **argv)
{
    pthread_t consumer, producer;
    int finalSync, finalDelayConsumer, finalFull;
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