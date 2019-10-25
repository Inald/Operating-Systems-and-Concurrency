#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define NUMBER_OF_JOBS 1000

sem_t sSync, sDelayConsumer;
int sharedCounter = 0;

/*displays the exact number of elements currently in buffer
everytime an element is added to or removed from the buffer*/
void visualisation(){
    printf("iIndex = %d\n", sharedCounter);
}

/*removes elements and decrements the shared counter for
every element removed */
void * consumerFunc(){
    int i = 0;
    sem_wait(&sDelayConsumer);
    while(i < NUMBER_OF_JOBS){
        sem_wait(&sSync);
        sharedCounter--;
        i++;
        visualisation();
        sem_post(&sSync);
        if(sharedCounter == 0){
            sem_wait(&sDelayConsumer);
        }
    }
}

/* adds elements and increments the shared counter for every
element added*/
void * producerFunc(){
    int i = 0;
    while(i < NUMBER_OF_JOBS){
        sem_wait(&sSync);
        sharedCounter++;
        i++;
        visualisation();
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
    sem_init(&sDelayConsumer, 0 ,1);
    pthread_create(&producer, NULL, producerFunc, NULL);
    pthread_create(&consumer, NULL, consumerFunc, NULL);
    pthread_join(consumer, NULL);
    pthread_join(producer, NULL);
    sem_getvalue(&sSync, &finalSync);
    sem_getvalue(&sDelayConsumer,&finalDelayConsumer);
    printf("sSync = %d, sDelayConsumer = %d\n", finalSync, finalDelayConsumer);

    return 0;
}