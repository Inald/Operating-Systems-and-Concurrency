#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

void algorithmRR(struct element **head, struct element **tail)
{
pid_t pid;
  int i, pBurst, rBurst, response = 0, turnAround = 0, sumTurnAround = 0;
  struct process *currentProcess;
  struct timeval *createdPtr, *finishPtr;
  while(*head)
  {
    //Get the head process of the list to run
    currentProcess = ((struct process *) removeFirst(head, tail));
    //Pointers to process times
    createdPtr = &(currentProcess -> oTimeCreated);
    finishPtr = &(currentProcess -> oMostRecentTime);
    //Run the process
    runPreemptiveJob(currentProcess, createdPtr, finishPtr);
    //Set variables to process details
    pid = currentProcess -> iProcessId;
    pBurst = currentProcess ->iPreviousBurstTime;
    rBurst = currentProcess ->iRemainingBurstTime;
    //Turnaround is difference in time from creation to current time
    turnAround += getDifferenceInMilliSeconds(*createdPtr, *finishPtr);
    if((currentProcess -> iRemainingBurstTime) <= 0)
    {
        //Sum individual turnaround times to calculate average
        sumTurnAround += turnAround;
        printf("Process ID = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Response Time = %d, Turnaround Time = %d\n", pid, pBurst, rBurst, response, turnAround);
        free(currentProcess);
    }
    else
    {
        printf("Process ID = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Response Time = %d\n", pid, pBurst, rBurst, response);
        addLast(currentProcess, head, tail);
    }
    //Response time is difference in burst times
    response += pBurst - rBurst;
  }
  //Calculate and print averages for response time and turn around time
  printf("Average response time = %f\nAverage turnaround time = %f\n", (double)response/(double)NUMBER_OF_JOBS, (double)sumTurnAround/(double)NUMBER_OF_JOBS);

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
  printf("END\n\n");
  //Apply Round Robin algorithm to elements in linked list
  algorithmRR(head, tail);
}

int main()
{
    createJobs();
    return 0;
}