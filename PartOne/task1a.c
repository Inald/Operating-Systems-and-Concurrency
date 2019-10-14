#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

// Implementation of the FCFS scheduling algorithm (by Michael and Inald)
// OSC Coursework - Part One - Task 1a

void algorithmFCFS(struct element **head, struct element **tail)
{
  pid_t pid;
  int i, pBurst, nBurst, response = 0, turnAround = 0, sumResponse = 0, sumTurnAround = 0;
  struct process *currentProcess;
  struct timeval *createdPtr, *finishPtr;
  //Remove in FCFS style - FIFO
  while(*head)
  {
    currentProcess = ((struct process *) removeFirst(head, tail));
    //Pointers to process times
    createdPtr = &(currentProcess -> oTimeCreated);
    finishPtr = &(currentProcess -> oMostRecentTime);
    //Run the process
    runNonPreemptiveJob(currentProcess, createdPtr, finishPtr);
    //Set variables to process details
    pid = currentProcess -> iProcessId;
    pBurst = currentProcess ->iPreviousBurstTime;
    nBurst = currentProcess ->iRemainingBurstTime;
    //Turnaround is difference in time between starting and finishing the process
    turnAround += getDifferenceInMilliSeconds(*createdPtr, *finishPtr);
    sumTurnAround += turnAround;
    printf("Process ID = %d, Previous Burst Time = %d, New Burst Time = %d, Response Time = %d, Turn Around Time = %d\n", pid, pBurst, nBurst, response, turnAround);
    //Calculate total sums of response time and turnaround times
    sumResponse += response;
    //Response time is difference in burst time
    response += pBurst - nBurst;
    free(currentProcess);
  }
  //Calculate and return averages for response time and turn around time
  printf("Average response time = %f\nAverage turn around time = %f\n", (double)sumResponse/(double)NUMBER_OF_JOBS, (double)sumTurnAround/(double)NUMBER_OF_JOBS);
}
void createJobs()
{
  int i;
  struct element *ptrH = NULL, *ptrT = NULL;
  struct element **head = &ptrH;
  struct element **tail = &ptrT;
  for(i = 1; i <= NUMBER_OF_JOBS; i++)
  {
    //Add process to the linked list
    addLast(generateProcess(), head, tail);
  }
  //Apply FCFS algorithm to elements in linked list
  algorithmFCFS(head, tail);
}
int main()
{
  createJobs();
  return 0; 
}


