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
sem_t queueSizes[MAX_PRIORITY * sizeof(sem_t)];

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
            printf("Consumer - synced correctly\n");
            sem_getvalue(&queueSizes[i], &count);
            printf("Consumer - got value of queueSizes[%d] as %d\n", i, count);
            if(count < MAX_BUFFER_SIZE)
            {
                printf("Consumer - entered if statement with count as %d\n", count);
                if(*priorityHeadArray[i])
                {   
                    printf("Consumer - head is not null\n");
                    firstProcess = ((struct process *)removeFirst(priorityHeadArray[i], priorityTailArray[i]));
                    printf("Consumer - removed process %d with priority %d correctly\n", firstProcess->iProcessId, firstProcess->iPriority);
                    start = firstProcess -> oTimeCreated;
                    end = firstProcess -> oMostRecentTime;
                    printf("Consumer - assigned timevals correctly\n");
                    runJob(firstProcess, &start, &end);
                    printf("Consumer - run process correctly\n");
                    firstProcess = processJob(cID, firstProcess, start, end);
                    if(firstProcess)
                    {
                        printf("Consumer - firstprocess not null correctly\n");
                        addLast(firstProcess, priorityHeadArray[i], priorityTailArray[i]);
                        printf("Consumer - added process back correctly\n");
                        sem_wait(&queueSizes[i]);
                        printf("Consumer - waited queueSizes correctly\n");
                        consumed--;
                    }
                    else
                    {
                        consumed++;
                        sem_post(&queueSizes[i]);
                        printf("Consumer - posted queueSizes correctly\n");
                    }
                }    
            }
            else if(consumed < NUMBER_OF_JOBS && producerAwake == 0)
            {
                producerAwake = 1;
                sem_post(&sDelayProducer);
                printf("Consumer - post synced correctly\n");
            }       
            sem_post(&sSync);
            
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
        printf("Producer - synced correctly\n");
        newProcess = generateProcess();
        printf("Producer - generated process correctly\n");
        priority = newProcess -> iPriority;
        printf("Producer - assigned priority correctly\n");
        sem_getvalue(&queueSizes[priority], &count);
        printf("Producer, value of queueSizes[%d] = %d\n", priority, count);
        if(count > 0)
        {
            printf("Producer - entered loop with count as %d\n", count);
            addLast(newProcess, priorityHeadArray[priority], priorityTailArray[priority]);
            printf("Producer - added process %d to priority %d correctly\n", newProcess->iProcessId, priority);
            produced++;
            sem_wait(&queueSizes[priority]);
            printf("Producer - waited queueSizes[%d] correctly\n", priority);
        }
        else if(produced < NUMBER_OF_JOBS && producerAwake == 1)
        {
            producerAwake = 0;
            sem_wait(&sDelayProducer);
            printf("Producer - waited producer correctly\n");
        }
        sem_post(&sSync);
        printf("Producer - posted synced correctly\n");
    }
}
int main(int argc, char **argv)
{
    pthread_t consumer, producer;
    int finalSync, finalDelayProducer;
    for(int i = 0; i < MAX_PRIORITY; i++)
    {
        priorityHeadArray[i] = priorityTailArray[i] = malloc(sizeof(struct element **));
        sem_init(&queueSizes[i], 0, MAX_BUFFER_SIZE);
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
    printf("TEST -- all semaphores initialised correctly\n");
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    //Final values of semapores
    sem_getvalue(&sSync, &finalSync);
    sem_getvalue(&sDelayProducer,&finalDelayProducer);
    printf("sSync = %d, sDelayProducer = %dn", finalSync, finalDelayProducer);
    return 0;
}
