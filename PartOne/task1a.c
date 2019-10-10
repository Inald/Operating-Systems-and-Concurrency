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
  int pBurst, nBurst, response, turnAround, i;
  double avgResponse, avgTurnAround;
  struct element* processList = (element*)malloc(sizeof(element)*NUMBER_OF_JOBS);
  for(i = 0; i < NUMBER_OF_JOBS; i++)
  {
  }
    
  free(processList);
  printf("Process ID = %d, Previous Burst Time = %d, New Burst Time = %d, Response Time = %d, Turn Around Time = %d\n", pid, pBurst, nBurst, response, turnAround);
  printf("Average response time = %f\n Average turn around time = %f\n", avgResponse, avgTurnAround);
  return 0;
}
