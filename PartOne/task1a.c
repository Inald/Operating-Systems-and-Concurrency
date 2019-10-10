#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
  pid_t pid;
  int pBurst, nBurst, response, turnAround;
  double avgResponse, avgTurnAround;
  
  printf("Process ID = %d, Previous Burst Time = %d, New Burst Time = %d, Response Time = %d, Turn Around Time = %d\n", pid, pBurst, nBurst, response, turnAround);
<<<<<<< HEAD
  printf("Average response time = %f\n Average turn around time = %f\n", avgResponse, avgTurnAround);
=======
  printf("Average response time = %f\nAverage turn around time = %f\n", avgResponse, avgTurnAround);
>>>>>>> c4c1390cfeb54941f6464e4462e0805aa40848f6
  return 0;
}
