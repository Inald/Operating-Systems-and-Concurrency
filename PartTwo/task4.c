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
sem_t sSync, sDelayProducer;
int produced = 0, consumed = 0, producerAwake = 0;
int dAverageResponseTime = 0;
int dAverageTurnAroundTime = 0;
//Create head and tail pointers to pointers for the linked list
struct element **priorityHeadArray[MAX_PRIORITY * sizeof(struct element **)];
struct element **priorityTailArray[MAX_PRIORITY * sizeof(struct element **)];
int queueSizes[MAX_PRIORITY * sizeof(int)];

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
		printf("Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Turnaround Time = %d\n", iConsumerId, pProcess->iProcessId, pProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", pProcess->iPriority, pProcess->iPreviousBurstTime, pProcess->iRemainingBurstTime, iTurnAroundTime);
		free(pProcess);
		return NULL;
	}
}

void * consumerFunc(void *id)
{
    int count, cID = *((int*)id); 
    struct process *firstProcess;
    struct timeval start, end;
    while(consumed < NUMBER_OF_JOBS)
    {
        //If elements exist, can remove
        for(int i = 0; i < MAX_PRIORITY; i++)
        {
            sem_wait(&sSync);
            if(queueSizes[i] > 0)
            {
                if(*priorityHeadArray[i])
                {   
                    firstProcess = ((struct process *)removeFirst(priorityHeadArray[i], priorityTailArray[i]));
                    start = firstProcess -> oTimeCreated;
                    end = firstProcess -> oMostRecentTime;
                    runJob(firstProcess, &start, &end);
                    firstProcess = processJob(cID, firstProcess, start, end);
                    if(firstProcess)
                    {
                        addLast(firstProcess, priorityHeadArray[i], priorityTailArray[i]);
                    }
                    else
                    {
                        consumed++;
                        queueSizes[i]--;
                    }
                }    
            }
            sem_post(&sSync);
            if(consumed < NUMBER_OF_JOBS && producerAwake == 0)
            {
                producerAwake = 1;
                sem_post(&sDelayProducer);
            }       
            
        }
    }
}


void * producerFunc(void *id)
{
    int priority, pID = (*(int *)id), count;
    struct process *newProcess;
    sem_wait(&sDelayProducer);
    while(produced < NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        newProcess = generateProcess();
        priority = newProcess -> iPriority;
        if(queueSizes[priority] < MAX_BUFFER_SIZE)
        {
            addLast(newProcess, priorityHeadArray[priority], priorityTailArray[priority]);
            produced++;
            queueSizes[priority]++;
        }
        sem_post(&sSync);
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
    int finalSync, finalDelayProducer;
    for(int i = 0; i < MAX_PRIORITY; i++)
    {
        priorityHeadArray[i] = priorityTailArray[i] = malloc(sizeof(struct element **));
    }
    sem_init(&sSync, 0 , 1);
    sem_init(&sDelayProducer, 0 , 1);
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
    printf("sSync = %d, sDelayProducer = %dn", finalSync, finalDelayProducer);
    return 0;
}
