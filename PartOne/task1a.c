#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "coursework.h"
#include "linkedlist.h"

// Implementation of the FCFS scheduling algorithm (by Michael and Inald)
// OSC Coursework - Part One - Task 1a
void createProcesses()
{
  int i;
  struct element *ptrH = NULL, *ptrT = NULL;
  struct element **head = &ptrH;
  struct element **tail = &ptrT;
  for(i = 1; i <= NUMBER_OF_JOBS; i++)
  {
    addLast(generateProcess(), head, tail); 
  }
}

int main()
{
  pid_t pid;
  int pBurst, nBurst, response, turnAround, i;
  double avgResponse, avgTurnAround;
  createProcesses();
  //printf("Process ID = %d, Previous Burst Time = %d, New Burst Time = %d, Response Time = %d, Turn Around Time = %d\n", pid, pBurst, nBurst, response, turnAround);
  //printf("Average response time = %f\nAverage turn around time = %f\n", avgResponse, avgTurnAround);
  return 0;
}


