#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

// Implementation of the FCFS scheduling algorithm (by Michael and Inald)
// OSC Coursework - Part One - Task 1a
struct element** createProcesses(struct element** processList, struct process* Listail, struct process* Listhead)
{
  int i;
  struct process* newProcess;
  for(i = 1; i < NUMBER_OF_JOBS; i++)
  {
    newProcess = generateProcess();
    printf("Address of processList: %x\n", &processList);
    addLast(newProcess,(struct element**) Listhead, (struct element**) Listail);
    listail = newProcess;
  }
  
  return processList;
}

int main()
{
  pid_t pid;
  int pBurst, nBurst, response, turnAround, i;
  double avgResponse, avgTurnAround;
  struct process* Listhead, Listail;
  Listhead = generateProcess();
  Listail = Listhead;
  //Create a pointer that points to a pointer of elements
  struct element** processList = createProcesses(processList,Listail,Listhead);
  printf("Process ID = %d, Previous Burst Time = %d, New Burst Time = %d, Response Time = %d, Turn Around Time = %d\n", pid, pBurst, nBurst, response, turnAround);
  printf("Average response time = %f\nAverage turn around time = %f\n", avgResponse, avgTurnAround);
  return 0;
}


