#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

//Semaphores for sync, delaying consumer
//Full and empty counting semaphores
sem_t sSync, sSyncPrint, sDelayProducer, sFull, sEmpty;
int produced = 0, consumed = 0, producerAwake = 0;
int dAverageResponseTime = 0;
int dAverageTurnAroundTime = 0;
//Represent MAX_PRIORITY amount of buffers in a array of linked lists
struct element **headArray[MAX_PRIORITY * sizeof(struct element **)];
struct element **tailArray[MAX_PRIORITY * sizeof(struct element **)];
int queueSizes[MAX_PRIORITY * sizeof(int)];

double calcAverage(int sum, int n)
{
  return ((double)sum/(double)n);
}

struct process * processJob(int iConsumerId, struct process * pProcess, struct timeval oStartTime, struct timeval oEndTime)
{
    sem_wait(&sSyncPrint);
	int iResponseTime;
	int iTurnAroundTime;
    if(pProcess->iPreviousBurstTime == pProcess->iInitialBurstTime && pProcess->iRemainingBurstTime > 0)
    {
        iResponseTime = getDifferenceInMilliSeconds(pProcess->oTimeCreated, oStartTime);	
        dAverageResponseTime += iResponseTime;
        printf("Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Response Time = %d\n", iConsumerId, pProcess->iProcessId, pProcess->iPriority < MAX_PRIORITY / 2	 ? "FCFS" : "RR",pProcess->iPriority, pProcess->iPreviousBurstTime, pProcess->iRemainingBurstTime, iResponseTime);
        return pProcess;
    } else if(pProcess->iPreviousBurstTime == pProcess->iInitialBurstTime && pProcess->iRemainingBurstTime == 0)
    {
        iResponseTime = getDifferenceInMilliSeconds(pProcess->oTimeCreated, oStartTime);	
        dAverageResponseTime += iResponseTime;
        iTurnAroundTime = getDifferenceInMilliSeconds(pProcess->oTimeCreated, oEndTime);
        dAverageTurnAroundTime += iTurnAroundTime;
        printf("Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Response Time = %d, Turnaround Time = %d\n", iConsumerId, pProcess->iProcessId, pProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", pProcess->iPriority, pProcess->iPreviousBurstTime, pProcess->iRemainingBurstTime, iResponseTime, iTurnAroundTime);
        free(pProcess);
        return NULL;
    } else if(pProcess->iPreviousBurstTime != pProcess->iInitialBurstTime && pProcess->iRemainingBurstTime > 0)
    {
        printf("Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d\n", iConsumerId, pProcess->iProcessId, pProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", pProcess->iPriority, pProcess->iPreviousBurstTime, pProcess->iRemainingBurstTime);
        return pProcess;
    } else if(pProcess->iPreviousBurstTime != pProcess->iInitialBurstTime && pProcess->iRemainingBurstTime == 0)
    {
        iTurnAroundTime = getDifferenceInMilliSeconds(pProcess->oTimeCreated, oEndTime);
        dAverageTurnAroundTime += iTurnAroundTime;
        printf("Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Turnaround Time = %d\n", iConsumerId, pProcess->iProcessId, pProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", pProcess->iPriority, pProcess->iPreviousBurstTime, pProcess->iRemainingBurstTime, iTurnAroundTime);
        free(pProcess);
        return NULL;
    }
}

void * consumerFunc(void *id)
{
    int cID = *((int*)id), currentPriority = 0; 
    struct process *firstProcess;
    struct timeval start, end;
    while(consumed < NUMBER_OF_JOBS)
    {
        sem_wait(&sFull);
        sem_wait(&sSync);
        
        sem_post(&sSync);
        sem_post(&sEmpty);
    }
}



void * producerFunc(void *id)
{
    int pID = (*(int *)id), priority;
    struct process *newProcess;
    while(produced < NUMBER_OF_JOBS)
    {
        sem_wait(&sEmpty);
        sem_wait(&sSync);
        newProcess = generatedProcess();
        priority = newProcess -> iPriority;
        addLast(newProcess, headArray[priority], tailArray[priority]);
        sem_post(&sSync);
        sem_post(&sFull);


    }
}
int main(int argc, char **argv)
{
    pthread_t consumer, producer;
    int finalSync, finalFull, finalEmpty;
    for(int i = 0; i < MAX_PRIORITY; i++)
    {
        headArray[i] = tailArray[i] = malloc(sizeof(struct element **));
    }
    sem_init(&sSync, 0 , 1);
    sem_init(&sDelayProducer, 0 , 1);
    sem_init(&sSyncPrint, 0, 1);
    sem_init(&sFull, 0, 0);
    sem_init(&sEmpty, 0, MAX_PRIORITY);
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
    sem_getvalue(&sFull, &finalFull);
    sem_getvalue(&sEmpty, &finalEmpty);
    printf("sSync = %d, sFull = %d, sEmpty = %d\n", finalSync, finalFull, finalEmpty);
    for(int i = 0; i < MAX_PRIORITY; i++){
        free(headArray[i]);
        free(tailArray[i]);
    
    }
    return 0;
}
