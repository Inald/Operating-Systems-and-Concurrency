#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

// Implementation of the FCFS scheduling algorithm (by Michael and Inald)
// OSC Coursework - Part One - Task 1a

int main()
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
    createdPtr = &(newProcess -> oTimeCreated);
    finishPtr = &(newProcess -> oMostRecentTime);
    addLast(newProcess, head, tail);
    runNonPreemptiveJob(newProcess, createdPtr, finishPtr);
    pid = newProcess -> iProcessId;
    pBurst = newProcess ->iPreviousBurstTime;
    nBurst = newProcess ->iRemainingBurstTime;
    response = turnAround;
    turnAround += (newProcess -> iPreviousBurstTime);
    sumResponse += response;
    sumTurnAround += turnAround;
    printf("Process ID = %d, Previous Burst Time = %d, New Burst Time = %d, Response Time = %d, Turn Around Time = %d\n", pid, pBurst, nBurst, response, turnAround);
    free(newProcess);
  }
  printf("Average response time = %f\nAverage turn around time = %f\n", (double)sumResponse/(double)NUMBER_OF_JOBS, (double)sumTurnAround/(double)NUMBER_OF_JOBS);
  //Remove in FCFS style - FIFO
  for(i = 1; i <= NUMBER_OF_JOBS; i++)
  {
     removeFirst(head, tail);
  }
  return 0;
}


