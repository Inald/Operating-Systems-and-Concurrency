#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

// Implementation of the FCFS scheduling algorithm (by Michael and Inald)
// OSC Coursework - Part One - Task 1a
<<<<<<< HEAD
struct element** createProcessses(struct element** processList){

  int i = 0;
  struct process* newProcess;
  
  for(i = 0; i < NUMBER_OF_JOBS; i++)
  {
    newProcess = (generateProcess() + i * sizeof(struct element**));
    printf("processID = %x\n", newProcess -> iProcessId);
    addLast(NULL,(struct element**)newProcess, (struct element**) newProcess);
  }
=======
struct element** createProcesses(struct element** processList)
{
  int i;
  struct process* newProcess;
  for(i = 0; i < NUMBER_OF_JOBS; i++)
  {
    newProcess = generateProcess() + i*sizeof(struct element**);
    //printf("GENERATED PROCESS - ID: %x, Priority:%d\n", newProcess -> iProcessId, newProcess ->iPriority);
    addLast(NULL, (struct element **)newProcess, (struct element **)newProcess);      
    printf("Address of list start: %x, Address of process: %x, ID: %d\n", &processList, &newProcess, newProcess -> iProcessId);
  }
  
>>>>>>> a458cbeb0111504f1fd98a1d2a0686a2fde1fc8a
  return processList;
}

int main()
{
  pid_t pid;
  int pBurst, nBurst, response, turnAround, i;
  double avgResponse, avgTurnAround;
<<<<<<< HEAD
  struct element** processList;
  processList = createProcessses(processList);
  
  //free(processList);
=======
  //Create a pointer that points to a pointer of elements
  struct element** processList = createProcesses(processList);;
>>>>>>> a458cbeb0111504f1fd98a1d2a0686a2fde1fc8a
  printf("Process ID = %d, Previous Burst Time = %d, New Burst Time = %d, Response Time = %d, Turn Around Time = %d\n", pid, pBurst, nBurst, response, turnAround);
  printf("Average response time = %f\nAverage turn around time = %f\n", avgResponse, avgTurnAround);
  return 0;
}

<<<<<<< HEAD
=======

>>>>>>> a458cbeb0111504f1fd98a1d2a0686a2fde1fc8a
