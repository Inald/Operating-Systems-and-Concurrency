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
sem_t sSync, sEmpty, sFull;
int produced = 0, consumed = 0;
int dAverageResponseTime = 0;
int dAverageTurnAroundTime = 0;
//Represent MAX_PRIORITY amount of buffers in a array of linked lists
struct element **headArray[MAX_PRIORITY];
struct element **tailArray[MAX_PRIORITY];
int queueSizes[MAX_PRIORITY];

double calcAverage(int sum, int n)
{
  return ((double)sum/(double)n);
}

void printSizes()
{
    for(int i = 0; i < MAX_PRIORITY; i++)
    {
        printf("%d ", queueSizes[i]);
    }
    printf("\n");
}
int sumSizes()
{
    int sum = 0;
    for(int i = 0; i < MAX_PRIORITY; i++)
    {
        sum += queueSizes[i];
    }
    return sum;
}

void printSems()
{
    int sema, semb;
    sem_getvalue(&sFull, &sema);
    sem_getvalue(&sEmpty, &semb);
    printf("Full = %d, Empty = %d\n", sema, semb);
}
struct process * processJob(int iConsumerId, struct process * pProcess, struct timeval oStartTime, struct timeval oEndTime)
{
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
        printf("Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d\n", iConsumerId, pProcess->iProcessId, pProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", pProcess->iPriority, pProcess->iPreviousBurstTime, pProcess->iRemainingBurstTime);
        free(pProcess);
        return NULL;
    }
}

void * consumerFunc(void *id)
{
    int count, currentPriority = 0, cID = (*(int *)id);
    struct process *firstProcess;
    struct timeval start, end;
    while(consumed < NUMBER_OF_JOBS)
    {
        sem_wait(&sFull);
        sem_wait(&sSync);
        firstProcess = removeFirst(headArray[currentPriority], tailArray[currentPriority]);
        sem_post(&sSync);
        if(firstProcess)
        {
            start = firstProcess -> oTimeCreated;
            end = firstProcess -> oMostRecentTime;
            runJob(firstProcess, &start, &end);
            sem_wait(&sSync);
            firstProcess = processJob(cID, firstProcess, start, end);
            if(firstProcess)
            {
                addLast(firstProcess, headArray[currentPriority], tailArray[currentPriority]);
            }
            else
            {
                consumed++;
                queueSizes[currentPriority]--;
                sem_post(&sEmpty);
            }
            sem_post(&sSync);
 
        }
        currentPriority++;
        if(currentPriority >= MAX_PRIORITY){currentPriority = 0;}
    }
    
}
void * producerFunc(void *id)
{
    struct process *newProcess;
    int priority, pID = (*(int *)id);
    while(produced < NUMBER_OF_JOBS)
    {
        newProcess = generateProcess();
        priority = newProcess -> iPriority;
        sem_wait(&sEmpty);
        sem_wait(&sSync);
        addLast(newProcess, headArray[priority], tailArray[priority]);
        sem_post(&sSync);
        sem_post(&sFull);
        printf("Producer %d, Process Id = %d, Priority = %d (%s), Previous Burst Time = %d, Remaining Burst Time = %d, Turnaround Time = %d\n", pID, newProcess->iProcessId, priority, newProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", newProcess->iPriority, newProcess->iPreviousBurstTime, newProcess->iRemainingBurstTime);
        produced++;
        queueSizes[priority]++;
    }
}
int main(int argc, char **argv)
{ 
    pthread_t consumer, producer;
    int finalSync, finalEmpty, finalFull, id;
    sem_init(&sSync, 0 , 1);
    sem_init(&sEmpty, 0, MAX_BUFFER_SIZE);
    sem_init(&sFull, 0, 0);
    for(int i = 0; i < MAX_PRIORITY; i++)
    {
        headArray[i] = tailArray[i] = calloc(1, sizeof(struct element *));
        queueSizes[i] = 0;
    }
    //Create producer and consumers
    for(int i = 0; i < NUMBER_OF_PRODUCERS; i++)
    {
        id = i;
        pthread_create(&producer, NULL, producerFunc, (void *)&id);
    }
    for(int i = 0; i < NUMBER_OF_CONSUMERS; i++)
    {
        id = i;
        pthread_create(&consumer, NULL, consumerFunc, (void *)&id);
    }
    pthread_join(consumer, NULL);
    pthread_join(producer, NULL);
    //Final values of semapores
    sem_getvalue(&sSync, &finalSync);
    sem_getvalue(&sFull, &finalFull);
    sem_getvalue(&sEmpty, &finalEmpty);
    printf("sSync = %d, sFull = %d, sEmpty = %d\n", finalSync, finalFull, finalEmpty);
    return 0;
}
