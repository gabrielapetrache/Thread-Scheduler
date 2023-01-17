**Petrache Gabriela,**
**322CA**

## Thread Scheduler

## Description:
* Implemented a thread scheduler in C, that can control the thread's execution in user-space, using a priority Round Robin
* The scheduler will create a dinamical library, which will store the planned threads


## Implementation:
* The thread structure contains the following:
	* the thread's id
	* the thread's handler function
	* the thread's priority
	* the thread's time slice
	* the thread's state (NEW, RUNNING, READY, WAITING, FINISHED)
	* the io event that the thread is waiting for
	* the semaphore of the thread

* The scheduler structure that stores the threads contains the following:
	* a priority queue, implemented using a linked list, in a Round Robin fashion
	* an array of all the threads
	* the current running thread which will be executed
	* the time quantum of the scheduler
	* the number of threads
	* the io number of supported events

* The scheduler will be able to perform the following functions:
	* INIT - initialize the scheduler at the beginning of the program
	* EXEC - will simulate the execution of a generic instruction
	* WAIT - the current running thread will enter the waiting state, waiting for an event or an I/O operation
	* SIGNAL - unlocks one or more threads that are waiting for the event given as an argument of the function
	* FORK - starts a new thread and adds it to the planner
	* END - frees the scheduler and terminates the execution of all threads