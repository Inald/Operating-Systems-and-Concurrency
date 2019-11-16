#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define NUMBER_OF_JOBS 1000

sem_t sSync, sDelayConsumer;
int sharedCounter = 0, produced = 0, consumed = 0;

/*displays the exact number of elements currently in buffer
everytime an element is added to or removed from the buffer*/
void visualisation(int sender, int ID)
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
    for(count = 0; count < sharedCounter; count++)
    {
        printf("*");
    }
    printf("\n");
}

/*removes elements and decrements the shared counter for
every element removed */
void * consumerFunc(){
    int temp = 0, cID = 1;
    sem_wait(&sDelayConsumer);
    while(consumed < NUMBER_OF_JOBS){
        sem_wait(&sSync);
        sharedCounter--;
        consumed++;
        temp = sharedCounter;
        visualisation(0, cID);
        sem_post(&sSync);
        //Sleep consumer
        if(temp == 0 && consumed != NUMBER_OF_JOBS){
            sem_wait(&sDelayConsumer);
        }
    }
}

/* adds elements and increments the shared counter for every
element added*/
void * producerFunc(){
    int pID = 1;
    while(produced < NUMBER_OF_JOBS){
        sem_wait(&sSync);
        sharedCounter++;
        produced++;
        visualisation(1, pID);
        //Wake up consumer
        if(sharedCounter == 1){
            sem_post(&sDelayConsumer);
        }
        sem_post(&sSync);
    }
}

int main(int argc, char **argv){
    pthread_t consumer, producer;
    int finalSync, finalDelayConsumer;
    sem_init(&sSync, 0 ,1);
    sem_init(&sDelayConsumer, 0 ,0);
    pthread_create(&producer, NULL, producerFunc, NULL);
    pthread_create(&consumer, NULL, consumerFunc, NULL);
    pthread_join(consumer, NULL);
    pthread_join(producer, NULL);
    sem_getvalue(&sSync, &finalSync);
    sem_getvalue(&sDelayConsumer,&finalDelayConsumer);
    printf("sSync = %d, sDelayConsumer = %d\n", finalSync, finalDelayConsumer);
    return 0;
}