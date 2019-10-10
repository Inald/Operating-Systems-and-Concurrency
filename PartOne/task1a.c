#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

// Implementation of the FCFS scheduling algorithm (by Michael and Inald)
// OSC Coursework - Part One - Task 1a
struct element** createProcessses(struct element** processList){

  int i = 0;
  struct process* newProcess;
  
  for(i = 0; i < NUMBER_OF_JOBS; i++)
  {
    newProcess = (generateProcess() + i * sizeof(struct element**));
    printf("processID = %x\n", newProcess -> iProcessId);
    addLast(NULL,(struct element**)newProcess, (struct element**) newProcess);
  }
  return processList;
}

int main()
{
  pid_t pid;
  int pBurst, nBurst, response, turnAround, i;
  double avgResponse, avgTurnAround;
  struct element** processList;
  processList = createProcessses(processList);
  
  //free(processList);
  printf("Process ID = %d, Previous Burst Time = %d, New Burst Time = %d, Response Time = %d, Turn Around Time = %d\n", pid, pBurst, nBurst, response, turnAround);
  printf("Average response time = %f\n Average turn around time = %f\n", avgResponse, avgTurnAround);
  return 0;
}

