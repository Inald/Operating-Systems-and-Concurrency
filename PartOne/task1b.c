#include "coursework.h"
#include "linkedlist.h"
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


//Implementation of the Round Robin scheduling algorithm (by Michael and Inald)
double calcAverage(int sum, int n)
{
  return ((double)sum/(double)n);
}
void algorithmRR(struct element **head, struct element **tail)
{
  pid_t pid;
  int responseCount = 0, pBurst, rBurst, response = 0, turnAround = 0, sumResponse = 0, sumTurnAround = 0;
  struct process *currentProcess;
  struct timeval *createdPtr, *finishPtr;
  while(*head)
  {
    //Run processes until the list is empty
    currentProcess = ((struct process *) removeFirst(head, tail));
    //Pointers to process times
    createdPtr = &(currentProcess -> oTimeCreated);
    finishPtr = &(currentProcess -> oMostRecentTime);
    //Run the process pre-emptively
    runPreemptiveJob(currentProcess, createdPtr, finishPtr);
    //Set variables to process details
    pid = currentProcess -> iProcessId;
    pBurst = currentProcess ->iPreviousBurstTime;
    rBurst = currentProcess ->iRemainingBurstTime;
    response = turnAround;
    turnAround += getDifferenceInMilliSeconds(*createdPtr, *finishPtr);
    if(rBurst > 0)
    {
      printf("Process ID = %d, Previous Burst Time = %d, Remaining Burst Time = %d", pid, pBurst, rBurst);
      if(responseCount < NUMBER_OF_JOBS)
      {
        responseCount++;
        sumResponse += response;
        printf(", Response time = %d\n", response);
      }
      else{printf ("\n");}
      addLast(currentProcess, head, tail);
    }
    else
    {
      sumTurnAround += turnAround;
      printf("Process ID = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Turnaround time = %d\n", pid, pBurst, rBurst, turnAround);
    }
  }
  printf("Average response time: %f\nAverage turnaround time: %f\n", calcAverage(sumResponse, NUMBER_OF_JOBS), calcAverage(sumTurnAround, NUMBER_OF_JOBS));
}
void createJobs(struct element **head, struct element **tail)
{
  int i;
  for(i = 1; i <= NUMBER_OF_JOBS; i++)
  {
    //Add process to the linked list
    addLast(generateProcess(), head, tail);
  }
}
int main()
{
  //Create head and tail pointers to pointers for the linked list
  struct element *ptrH = NULL, *ptrT = NULL;
  struct element **head = &ptrH, **tail = &ptrT;
  createJobs(head, tail);
  //Apply Round Robin algorithm to elements in linked list
  algorithmRR(head, tail);
  return 0; 
}


