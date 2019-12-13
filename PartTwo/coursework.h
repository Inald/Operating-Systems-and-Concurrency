/**************************************************************************************************************************
			DO NOT CHANGE THE CONTENTS OF THIS FILE FOR YOUR COURSEWORK. ONLY WORK WITH THE OFFICIAL VERSION
**************************************************************************************************************************/

#include <sys/time.h>

// Duration of the time slice for the round robin algorithm
#define TIME_SLICE 5

// Number of processes to create
#define NUMBER_OF_JOBS 100

// Maximum duration of the individual processes, in milli seconds. Note that the times themselves will be chosen at random in ]0,100]
#define MAX_BURST_TIME 50

// Maximum process priority 
#define MAX_PRIORITY 32

#define MAX_BUFFER_SIZE 10

#define MAX_NUMBER_OF_JOBS NUMBER_OF_JOBS

#define NUMBER_OF_PRODUCERS 1

#define NUMBER_OF_CONSUMERS 3

#define BOOST_INTERVAL 50

/* 
 * Definition of the structure containing the process characteristics. These should be sufficient for the full implementation of all tasks.
 */

struct process
{
	int iPreempt; 
	int iProcessId;
	struct timeval oTimeCreated;
	struct timeval oMostRecentTime; // most recent time the job finished running
	int iInitialBurstTime;
	int iPreviousBurstTime;
	int iRemainingBurstTime;
	int iPriority;
};

void runJob(struct process * pTemp, struct timeval * oStartTime, struct timeval * oEndTime);
void preemptJob(struct process * pTemp);
struct process * generateProcess();
long int getDifferenceInMilliSeconds(struct timeval start, struct timeval end);
void runProcess(struct process * oTemp, int iBurstTime, struct timeval * oStartTime, struct timeval * oEndTime);
void runNonPreemptiveJob(struct process * oTemp, struct timeval * oStartTime, struct timeval * oEndTime);
void runPreemptiveJob(struct process * oTemp, struct timeval * oStartTime, struct timeval * oEndTime);
