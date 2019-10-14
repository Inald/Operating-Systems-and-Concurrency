#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

void algorithmRR(struct element **head, struct element **tail)
{
pid_t pid;
  int i, pBurst, nBurst, response, turnAround = 0, sumResponse = 0, sumTurnAround = 0;
  struct process *currentProcess;
  struct timeval *createdPtr, *finishPtr;
  while(*head)
  {
    currentProcess = ((struct process *) removeFirst(head, tail));
    //Pointers to process times
    createdPtr = &(currentProcess -> oTimeCreated);
    finishPtr = &(currentProcess -> oMostRecentTime);
    //Run the process
    runPreemptiveJob(currentProcess, createdPtr, finishPtr);
    //Set variables to process details
    pid = currentProcess -> iProcessId;
    pBurst = currentProcess ->iPreviousBurstTime;
    nBurst = currentProcess ->iRemainingBurstTime;
    //Response time is previous turnAround time
    response += pBurst - nBurst;
    //TurnAround is previous turnAround + previous burst time
    turnAround += (currentProcess -> iPreviousBurstTime);
    if((currentProcess -> iRemainingBurstTime) <= 0)
    {
        printf("Process ID = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Turn Around Time = %d\n", pid, pBurst, nBurst, turnAround);
        free(currentProcess);
    }
    else
    {
        printf("Process ID = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Response Time = %d\n", pid, pBurst, nBurst, response);
        addLast(currentProcess, head, tail);
    }

    //Calculate total sums of response time and turnaround times
    sumResponse += response;
    sumTurnAround += turnAround;
  }
  //Calculate and return averages for response time and turn around time
  printf("Average response time = %f\nAverage turn around time = %f\n", (double)sumResponse/(double)NUMBER_OF_JOBS, (double)sumTurnAround/(double)NUMBER_OF_JOBS);

}
void createJobs()
{
  int i;
  struct process *newProcess;
  struct element *ptrH = NULL, *ptrT = NULL;
  struct element **head = &ptrH;
  struct element **tail = &ptrT;
  printf("PROCESS LIST: \n");
  for(i = 1; i <= NUMBER_OF_JOBS; i++)
  {
    newProcess = generateProcess();
    //Add process to the linked list
    addLast(newProcess, head, tail);
    printf("        Process ID: %d, Initial Burst Time: %d, Remaining Burst Time = %d\n", newProcess -> iProcessId, newProcess -> iInitialBurstTime, newProcess ->iRemainingBurstTime);    
  }
  printf("END\n");
  //Apply Round Robin algorithm to elements in linked list
  algorithmRR(head, tail);
}

int main()
{
    createJobs();
    return 0;
}