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
  int i, pBurst, nBurst, response, turnAround = 0, sumResponse = 0, sumTurnAround = 0;
  struct process *currentProcess;
  //Remove in FCFS style - FIFO
  for(i = 1; i <= NUMBER_OF_JOBS; i++)
  {
    currentProcess = ((struct process *) removeFirst(head, tail));
    //Set variables to process details
    pid = currentProcess -> iProcessId;
    pBurst = currentProcess ->iPreviousBurstTime;
    nBurst = currentProcess ->iRemainingBurstTime;
    //Response time is previous turnAround time
    response = turnAround;
    //TurnAround is previous turnAround + previous burst time
    turnAround += (currentProcess -> iPreviousBurstTime);
    //Calculate total sums of response time and turnaround times
    sumResponse += response;
    sumTurnAround += turnAround;
    printf("Process ID = %d, Previous Burst Time = %d, New Burst Time = %d, Response Time = %d, Turn Around Time = %d\n", pid, pBurst, nBurst, response, turnAround);
    free(currentProcess);
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
  struct timeval *createdPtr, *finishPtr;
  for(i = 1; i <= NUMBER_OF_JOBS; i++)
  {
    newProcess = generateProcess();
    //Pointers to process times
    createdPtr = &(newProcess -> oTimeCreated);
    finishPtr = &(newProcess -> oMostRecentTime);
    //Run the process
    runNonPreemptiveJob(newProcess, createdPtr, finishPtr);
    //Add process to the linked list
    addLast(newProcess, head, tail);
  }
  //Apply FCFS algorithm to elements in linked list
  algorithmFCFS(head, tail);
}
int main()
{
  createJobs();
  return 0; 
}


