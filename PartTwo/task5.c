#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "coursework.h"
#include "linkedlist.h"

//Semaphores for sync, delaying consumer
sem_t sSync, sEmpty, sFull;
int produced = 0, consumed = 0, dAverageResponseTime, dAverageTurnAroundTime, finishedCount = 0;
int totalLength = 0;
double averageLength = 0;
struct element *headArray[MAX_PRIORITY], *tailArray[MAX_PRIORITY];
struct process *runningProcesses[NUMBER_OF_CONSUMERS];
double calcAverage(int sum, int n)
{
  return ((double)sum/(double)n);
}

struct process *lowestPriorityRunningProcesses()
{
    struct process *lowestProcess = NULL;
    int lowestPriority = MAX_PRIORITY/2;
    for(int i = 0; i < NUMBER_OF_CONSUMERS - 1; i++)
    {
        if(runningProcesses[i])
        {
            if(lowestPriority < runningProcesses[i] -> iPriority)
            {
                lowestPriority = runningProcesses[i] -> iPriority;
                lowestProcess = runningProcesses[i];
            }
        }
    }
    return lowestProcess;
}
void testLowerPriorityFCFS(struct process *job)
{
    struct process *process = lowestPriorityRunningProcesses();
    if(process)
    {
        if(process -> iPriority > job -> iPriority)
        {
            preemptJob(process);
            printf("Pre-Empted Job: Pre-Empted Process ID = %d, Pre-Empted Priority = %d, New Process ID = %d\n", process ->iProcessId, process->iPriority, job->iProcessId);

        }  
    }
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
		printf("Consumer %d, Process Id = %d (%s), Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Turnaround Time = %d\n", iConsumerId, pProcess->iProcessId, pProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", pProcess->iPriority, pProcess->iPreviousBurstTime, pProcess->iRemainingBurstTime, iTurnAroundTime);
		free(pProcess);
        return NULL;
	}
}

 void * consumerFunc(void *id)
 {
     int count,cID = (*(int *) id), currentPriority = 0;
     struct process *firstProcess, *processRun; 
     struct timeval start, end;
     while(consumed < MAX_NUMBER_OF_JOBS)
     {
        sem_wait(&sFull);
        sem_wait(&sSync);
        for(int i = 0; i < MAX_PRIORITY; i++){
            if(headArray[i]){
                currentPriority = i;
                break;
            }
        }
        firstProcess = removeFirst(&headArray[currentPriority], &tailArray[currentPriority]);
        sem_post(&sSync);
        if(firstProcess)
        {
            //check for values of time
            sem_wait(&sSync);
            runJob(firstProcess, &start, &end);
            runningProcesses[cID] = firstProcess;
            processRun = processJob(cID, firstProcess, start, end);
            if(processRun){
                addLast(processRun, &headArray[currentPriority], &tailArray[currentPriority]);
                sem_post(&sFull);
            }else{
                consumed++;
                sem_post(&sEmpty);
            }
            sem_post(&sSync);
            runningProcesses[cID] = NULL;
        }
     }
     if(finishedCount < NUMBER_OF_CONSUMERS-1){
        sem_wait(&sSync);
        sem_post(&sFull);
        finishedCount++;
        sem_post(&sSync);
     }
}


void * producerFunc(void *id)
{
    struct process *newProcess;
    int count, pID = (*(int *)id);
    
    while(produced < MAX_NUMBER_OF_JOBS)
    {
        sem_wait(&sEmpty);
        sem_wait(&sSync);
        newProcess = generateProcess();
        addLast(newProcess, &headArray[newProcess -> iPriority], &tailArray[newProcess -> iPriority]);
        produced++;
        printf("Producer %d, Process Id = %d, Priority = %d (%s), Initial Burst Time = %d\n", pID, newProcess->iProcessId, newProcess->iPriority, newProcess->iPriority < MAX_PRIORITY / 2 ? "FCFS" : "RR", newProcess->iInitialBurstTime);
        if(newProcess -> iPriority < MAX_PRIORITY/2){testLowerPriorityFCFS(newProcess);}        
        sem_post(&sSync);
        sem_post(&sFull);
    }
}

void * boosterFunc()
{
    struct process *checkProcess;
    int currentPriority = MAX_PRIORITY/2;
    int highestPriority = MAX_PRIORITY/2;
    struct timeval mostRecent, current;
    while(consumed < MAX_NUMBER_OF_JOBS)
    {
        sem_wait(&sSync);
        checkProcess = removeFirst(&headArray[currentPriority], &tailArray[currentPriority]);
        if(checkProcess)
        {
            mostRecent = checkProcess -> oMostRecentTime;
            gettimeofday(&current, NULL);
            if(getDifferenceInMilliSeconds(mostRecent, current) >= BOOST_INTERVAL)
            {
                printf("Boost priority: Process ID = %d, Priority = %d, New Priority = %d\n", checkProcess -> iProcessId, checkProcess -> iPriority, highestPriority);
                checkProcess -> iPriority = highestPriority;
                addFirst(checkProcess, &headArray[highestPriority], &tailArray[highestPriority]);
            }
            else
            {
                addFirst(checkProcess, &headArray[currentPriority], &tailArray[currentPriority]);
            }
        }
        currentPriority++;
        if(currentPriority >= MAX_PRIORITY)
        {
            currentPriority = MAX_PRIORITY/2;
        }
        sem_post(&sSync);
    }
}

int main(int argc, char **argv)
{
    pthread_t consumer[NUMBER_OF_CONSUMERS], producer, booster;
    int finalSync, finalEmpty, finalFull, producerID = 0, conIds[NUMBER_OF_CONSUMERS];
    sem_init(&sSync, 0 , 1);
    sem_init(&sEmpty, 0 , MAX_BUFFER_SIZE);
    sem_init(&sFull, 0, 0);
    pthread_create(&producer, NULL, producerFunc, &producerID);
    pthread_create(&booster, NULL, boosterFunc, NULL);
    for(int i = 0; i < NUMBER_OF_CONSUMERS; i++)
    {
        conIds[i] = i;
        pthread_create(&consumer[i], NULL, consumerFunc, &conIds[i]);
    }
    for(int j = 0; j < NUMBER_OF_CONSUMERS; j++)
    {
        pthread_join(consumer[j], NULL);
    }
    pthread_join(producer, NULL);
    pthread_join(booster, NULL);
    //Final values of semapores
    sem_getvalue(&sSync, &finalSync);
    sem_getvalue(&sEmpty, &finalEmpty);
    sem_getvalue(&sFull, &finalFull);
    printf("sSync = %d, sEmpty = %d, sFull = %d\n", finalSync, finalEmpty, finalFull);
    printf("Average Reponse Time = %f\n", calcAverage(dAverageResponseTime, NUMBER_OF_JOBS));
    printf("Average Turnaround Time = %f\n", calcAverage(dAverageTurnAroundTime, NUMBER_OF_JOBS));
    return 0;
}
