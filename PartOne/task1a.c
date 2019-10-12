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
  int i;
  //Remove in FCFS style - FIFO
  for(i = 1; i <= NUMBER_OF_JOBS; i++)
  {
    removeFirst(head, tail);
  }
}
void createJobs()
{
  pid_t pid;
  int i, pBurst, nBurst, response, turnAround = 0, sumResponse = 0, sumTurnAround = 0;
  struct process *newProcess;
  struct element *ptrH = NULL, *ptrT = NULL;
  struct element **head = &ptrH;
  struct element **tail = &ptrT;
  struct timeval *createdPtr, *finishPtr;
  for(i = 1; i <= NUMBER_OF_JOBS; i++)
  {
    newProcess = generateProcess();
    //pointers to process times
    createdPtr = &(newProcess -> oTimeCreated);
    finishPtr = &(newProcess -> oMostRecentTime);
    //add process to the linked list
    runNonPreemptiveJob(newProcess, createdPtr, finishPtr);
    addLast(newProcess, head, tail);
    //run the process
    //set variables to process details
    pid = newProcess -> iProcessId;
    pBurst = newProcess ->iPreviousBurstTime;
    nBurst = newProcess ->iRemainingBurstTime;
    //response time is previous turnAround time
    response = turnAround;
    //turnAround is previous turnAround + previous burst time
    turnAround += (newProcess -> iPreviousBurstTime);
    //calculate total sums of response time and turnaround times
    sumResponse += response;
    sumTurnAround += turnAround;
    printf("Process ID = %d, Previous Burst Time = %d, New Burst Time = %d, Response Time = %d, Turn Around Time = %d\n", pid, pBurst, nBurst, response, turnAround);
    free(newProcess);
  }
  //Apply FCFS algorithm to elements in linked list
  algorithmFCFS(head, tail);
  //Calculate and return averages for response time and turn around time
  printf("Average response time = %f\nAverage turn around time = %f\n", (double)sumResponse/(double)NUMBER_OF_JOBS, (double)sumTurnAround/(double)NUMBER_OF_JOBS);
}
int main()
{
  createJobs();
  return 0; 
}


