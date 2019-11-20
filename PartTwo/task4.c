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
sem_t sSync, sPrintSync, sEmpty, sFull;
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
    sem_wait(&sSync);
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
    int count, cID = (*(int *)id);
    struct process *firstProcess;
    struct timeval start, end;
    while(consumed < NUMBER_OF_JOBS)
    {
        sem_wait(&sFull);
        for(int i = 0; i < MAX_PRIORITY; i++)
        {
            sem_wait(&sSync);
            firstProcess = removeFirst(headArray[i], tailArray[i]);
            sem_post(&sSync);
            if(firstProcess)
            {
                start = firstProcess -> oTimeCreated;
                end = firstProcess -> oMostRecentTime;
                runJob(firstProcess, &start, &end);
                firstProcess = processJob(cID, firstProcess, start, end);
                sem_post(&sSync);
                if(firstProcess)
                {
                    sem_wait(&sSync);
                    addLast(firstProcess, headArray[i], tailArray[i]);
                    sem_post(&sSync);
                }
                else
                {
                    sem_wait(&sSync);
                    consumed++;
                    printf("produced: %d, consumed: %d\n", produced, consumed);
                    sem_post(&sSync);
                    queueSizes[i]--;
                }
            }
        }
        sem_post(&sEmpty);
    }
    
}
void * producerFunc()
{
    struct process *newProcess;
    int priority;
    while(produced < NUMBER_OF_JOBS)
    {
        sem_wait(&sEmpty);
        if(sumSizes() < MAX_BUFFER_SIZE && produced < NUMBER_OF_JOBS)
        {
            newProcess = generateProcess();
            priority = newProcess -> iPriority;
            if(queueSizes[priority] < MAX_BUFFER_SIZE)
            {
                sem_wait(&sSync);
                addLast(newProcess, headArray[priority], tailArray[priority]);
                produced++;
                sem_post(&sSync);
                queueSizes[priority]++;
                printf("Producer %d, Process Id = %d, Priority = %d (%s), Previous Burst Time = %d, Remaining Burst Time = %d\n", 1, newProcess->iProcessId, priority, newProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", newProcess->iPreviousBurstTime, newProcess->iRemainingBurstTime);
            }
        }
        sem_post(&sFull);
    }
}
int main(int argc, char **argv)
{ 
    pthread_t consumer, producer;
    int finalSync, finalEmpty, finalFull, finalPrint, id;
    sem_init(&sSync, 0 , 1);
    sem_init(&sEmpty, 0, MAX_BUFFER_SIZE);
    sem_init(&sFull, 0, 0);
    sem_init(&sPrintSync, 0, 1);
    for(int i = 0; i < MAX_PRIORITY; i++)
    {
        headArray[i] = tailArray[i] = calloc(1, sizeof(struct element *));
        queueSizes[i] = 0;
    }
    //Create producer and consumers
    pthread_create(&producer, NULL, producerFunc, NULL);
    for(int i = 1; i <= NUMBER_OF_CONSUMERS; i++)
    {
        id = i;
        pthread_create(&consumer, NULL, consumerFunc, (void *)&id);
    }
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    //Final values of semapores
    sem_getvalue(&sSync, &finalSync);
    sem_getvalue(&sFull, &finalFull);
    sem_getvalue(&sEmpty, &finalEmpty);
    sem_getvalue(&sPrintSync, &finalPrint);
    printf("sSync = %d, sPrintSync = %d, sFull = %d, sEmpty = %d\n", finalSync, finalPrint, finalFull, finalEmpty);
    return 0;
}
