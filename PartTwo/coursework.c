/**************************************************************************************************************************
			DO NOT CHANGE THE CONTENTS OF THIS FILE FOR YOUR COURSEWORK. ONLY WORK WITH THE OFFICIAL VERSION
**************************************************************************************************************************/

#include <stdlib.h>
#include <sys/time.h>
#include "coursework.h"
#include <stdio.h>

int iPid = 0;

/*
 * Function that generates a single job and initialises the fields. Process will have an increasing job id, reflecting the order in which they were created.
 * Note that the objects returned are allocated in dynamic memory, and that the caller is responsible for free-ing the memory when it is no longer in use.  
 *
 * REMARK: note that the random generator will generate a fixed sequence of random numbers. I.e., every time the code is run, the times that are generated will be the same, although the individual 
 * numbers themselves are "random". This is achieved by seeding the generator (by default), and is done to facilitate debugging if necessary.
 */

struct process * generateProcess()
{	
	struct process * pTemp = (struct process *) malloc (sizeof(struct process));
	if(pTemp == NULL)
	{
		printf("Malloc Failed\n");
		exit(-1);
	}
	pTemp->iPreempt = 0;
	pTemp->iProcessId = iPid++;
	pTemp->iRemainingBurstTime = pTemp->iPreviousBurstTime = pTemp->iInitialBurstTime = (rand() % MAX_BURST_TIME) + 1;
	pTemp->iPriority = (rand() % MAX_PRIORITY);
	gettimeofday(&(pTemp->oTimeCreated), NULL);
	return pTemp; 
}

/*
 * Function returning the time difference in milliseconds between the two time stamps, with start being the earlier time, and end being the later time.
 */
long int getDifferenceInMilliSeconds(struct timeval start, struct timeval end)
{
	long int iSeconds = end.tv_sec - start.tv_sec;
	long int iUSeconds = end.tv_usec - start.tv_usec;
 	long int mtime = (iSeconds * 1000 + iUSeconds / 1000.0);
	return mtime;
}


/*
 * Calls runPreemptive or runNonPreemptiveJob depending on the priority level. This function is meant to make the implementation of the coursework easier.
 */
void runJob(struct process * pTemp, struct timeval * oStartTime, struct timeval * oEndTime)
{
	if(pTemp->iPriority < MAX_PRIORITY / 2)
		runNonPreemptiveJob(pTemp, oStartTime, oEndTime);
	else
		runPreemptiveJob(pTemp, oStartTime, oEndTime);
}
/*
 * Function to call when simulating a non-preemptive job. This function will:
 * - change the state to running
 * - run the job
 * - set the burst time of the process to 0
 * - update the state to finished
 */
void runNonPreemptiveJob(struct process * pTemp, struct timeval * oStartTime, struct timeval * oEndTime)
{
	long int iDifference = 0;
	runProcess(pTemp, pTemp->iInitialBurstTime, oStartTime, oEndTime);
	pTemp->iRemainingBurstTime = 0;
}

/*
 * Function to call when simulating a NON-BLOCKING round robin job. This function will:
 * - calculate the (remaining) burst time
 * - set the state to running
 * - run the job
 * - reduce the burst time of the process with the time that it ran 
 * - change the state to finished if the burst time reaches 0, set it to ready if the process used its entire time slice and was taken off the CPU
 */
 
void runPreemptiveJob(struct process * pTemp, struct timeval * oStartTime, struct timeval * oEndTime)
{
	long int iDifference = 0;
	int iBurstTime = pTemp->iRemainingBurstTime > TIME_SLICE ? TIME_SLICE : pTemp->iRemainingBurstTime;
	runProcess(pTemp, iBurstTime, oStartTime, oEndTime);
	pTemp->iPreviousBurstTime = pTemp->iRemainingBurstTime;
	pTemp->iRemainingBurstTime -= iBurstTime;
	gettimeofday(&pTemp->oMostRecentTime, NULL);
}

/*
 * Simulates the job running on a CPU for a number of milliseconds; Note that the running of the process may be stopped 
 * if the flag iPreempt is set to 1.
 */
void runProcess(struct process * pTemp, int iBurstTime, struct timeval * oStartTime, struct timeval * oEndTime)
{
	struct timeval oCurrent;
	long int iDifference = 0;
	gettimeofday(oStartTime, NULL);
	do
	{	
		gettimeofday(&oCurrent, NULL);
		iDifference = getDifferenceInMilliSeconds((*oStartTime), oCurrent);
	} while(iDifference < iBurstTime && !pTemp->iPreempt);
	pTemp->iPreempt = 0;
	gettimeofday(oEndTime, NULL);
}

/**
* This function sets to 1 the value of iPreempt, so that, runProcess(..) stops running the process.
*/
void preemptJob(struct process * pTemp)
{
	pTemp->iPreempt = 1;
}