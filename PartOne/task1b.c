#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

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
  //Run processes until the list is empty
  while(*head)
  {
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
    if(rBurst <= 0)
    {
      //In the event that a process is finished within first time slice
      if(responseCount < NUMBER_OF_JOBS)
      {
        response = turnAround;
        //Count the first NUMBER_OF_JOBS to print response times
        responseCount++;
        sumResponse += response;
        printf("Process ID = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Response Time = %d,", pid, pBurst, rBurst, response);
      }
      else
      {
        //Sum individual turnaround times to calculate average
        sumTurnAround += turnAround;
        printf("Process ID = %d, Previous Burst Time = %d, Remaining Burst Time = %d,", pid, pBurst, rBurst);
        free(currentProcess);
      }
      printf(" Turnaround time = %d\n", turnAround);
    }
    else 
    {
      if(responseCount < NUMBER_OF_JOBS)
      {
        response = turnAround;
        //Count the first NUMBER_OF_JOBS to print response times
        responseCount++;
        sumResponse += response;
        printf("Process ID = %d, Previous Burst Time = %d, Remaining Burst Time = %d, Response Time = %d\n", pid, pBurst, rBurst, response);
      }
      else
      {
        printf("Process ID = %d, Previous Burst Time = %d, Remaining Burst Time = %d\n", pid, pBurst, rBurst);
      }
      turnAround += getDifferenceInMilliSeconds(*createdPtr, *finishPtr);
      addLast(currentProcess, head, tail);
    }
  }
  //Calculate and print averages for response time and turn around time
  printf("Average response time = %f\nAverage turn around time = %f\n", calcAverage(sumResponse, NUMBER_OF_JOBS), calcAverage(sumTurnAround, NUMBER_OF_JOBS));
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


