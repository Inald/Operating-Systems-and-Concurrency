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
  int responseCount = 0, pBurst, rBurst, priority, response = 0, turnAround = 0, sumResponse = 0, sumTurnAround = 0;
  struct process *currentProcess;
  struct timeval created, finished;
  while(*head)
  {
    //Run processes until the list is empty
    currentProcess = ((struct process *) removeFirst(head, tail));
    //Process times
    created = currentProcess -> oTimeCreated;
    finished = currentProcess -> oMostRecentTime;
    //Run the process pre-emptively
    runPreemptiveJob(currentProcess, &created, &finished);
    //Set variables to process details
    pid = currentProcess -> iProcessId;
    pBurst = currentProcess ->iPreviousBurstTime;
    rBurst = currentProcess ->iRemainingBurstTime;
    priority = currentProcess -> iPriority;
    //Response time of a process is same as current turnaround time of previous process
    response = turnAround;
    //Get difference in time between creation and most recent time.
    turnAround += getDifferenceInMilliSeconds(created, finished);
    printf("Process ID = %d, Priority = %d, Previous Burst Time = %d, Remaining Burst Time = %d", pid, priority, pBurst, rBurst);
    //Print response time if first time responding
    if(responseCount < MAX_NUMBER_OF_JOBS)
    {
      responseCount++;
      sumResponse += response;
      printf(", Response time = %d", response);
    }
    //If process isn't finished...
    if(rBurst > 0)
    {
      printf ("\n");
      //Add process back onto list
      addLast(currentProcess, head, tail);
    }
    else
    {
      sumTurnAround += turnAround;
      printf(", Turnaround time = %d\n", turnAround);
      free(currentProcess);
    }
  }
  //Calculate averages of turnaround/response
  printf("Average response time: %f\nAverage turnaround time: %f\n", calcAverage(sumResponse, MAX_NUMBER_OF_JOBS), calcAverage(sumTurnAround, MAX_NUMBER_OF_JOBS));
}
void createJobs(struct element **head, struct element **tail)
{
  int i, pID, priority, pBurst, rBurst;
  struct process* newProcess;
  printf("PROCESS LIST:\n");
  for(i = 1; i <= MAX_NUMBER_OF_JOBS; i++)
  {
    newProcess = generateProcess();
    pID = newProcess -> iProcessId;
    priority = newProcess -> iPriority;
    pBurst = newProcess -> iPreviousBurstTime;
    rBurst = newProcess -> iRemainingBurstTime;
    printf("      Process ID = %d, Priority = %d, Initial Burst Time = %d, Remaining Burst Time = %d\n", pID, priority, pBurst, rBurst);
    //Add process to the linked list in a FCFS fashion
    addLast(newProcess, head, tail);
  }
  printf("END\n\n");
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


