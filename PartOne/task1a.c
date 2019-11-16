#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

// Implementation of the FCFS scheduling algorithm (by Michael and Inald)

double calcAverage(int sum, int n)
{
  return ((double)sum/(double)n);
}

void algorithmFCFS(struct element **head, struct element **tail)
{
  pid_t pid;
  int pBurst, nBurst, response = 0, turnAround = 0, sumResponse = 0, sumTurnAround = 0;
  struct process *currentProcess;
  struct timeval createdPtr, recentPtr;
  //Run processes until list is empty
  while(*head)
  {
    currentProcess = ((struct process *) removeFirst(head, tail));
    createdPtr = (currentProcess -> oTimeCreated);
    recentPtr = (currentProcess -> oMostRecentTime);
    //Run the process non pre-emptively
    runNonPreemptiveJob(currentProcess, &createdPtr, &recentPtr);
    //Set variables to process details
    pid = currentProcess -> iProcessId;
    pBurst = currentProcess ->iPreviousBurstTime;
    nBurst = currentProcess ->iRemainingBurstTime;
    //Response is the same as turnaround for previous process
    response = turnAround;
    sumResponse += response;
    //Turnaround is difference in time between starting and finishing the process
    turnAround += getDifferenceInMilliSeconds(createdPtr, recentPtr);
    sumTurnAround += turnAround;
    printf("Process ID = %d, Previous Burst Time = %d, New Burst Time = %d, Response Time = %d, Turnaround Time = %d\n", pid, pBurst, nBurst, response, turnAround);
    free(currentProcess);
  }
  //Calculate and print averages for response time and turn around time
  printf("Average response time = %f\nAverage turnaround time = %f\n", calcAverage(sumResponse, NUMBER_OF_JOBS), calcAverage(sumTurnAround, NUMBER_OF_JOBS));
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
  //Apply FCFS algorithm to elements in linked list
  algorithmFCFS(head, tail);
  return 0; 
}
