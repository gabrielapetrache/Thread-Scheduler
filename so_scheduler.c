#include <semaphore.h>
#include <stdlib.h>
#include "so_scheduler.h"
#include "priorityqueue.h"

typedef struct {
	tid_t id;
	so_handler *handler;
	unsigned int priority;
	unsigned int time_left;
	unsigned int io;
	// ready, running, waiting, new, finished - 0, 1, 2, 3, 4
	unsigned int state;
	sem_t sem;
} thread_t;

typedef struct {
	PriorityQueue *queue;
	unsigned int time_quantum;
	unsigned int io;
	unsigned int num_threads;
	thread_t **threads; // array of threads
	thread_t *running; // running thread
} scheduler_t;

scheduler_t *scheduler;
unsigned int INIT;
void schedule_task(void);

int so_init(unsigned int time_quantum, unsigned int io)
{
	// check if arguments are valid
	if (time_quantum <= 0 || INIT == 1 || io > SO_MAX_NUM_EVENTS)
		return -1;

	// initialize scheduler
	INIT = 1;
	scheduler = (scheduler_t *)malloc(sizeof(scheduler_t));

	scheduler->time_quantum = time_quantum;
	scheduler->io = io;
	scheduler->queue = createPriorityQueue();
	scheduler->threads = (thread_t **)malloc(sizeof(thread_t *) * 100);

	scheduler->running = NULL;
	scheduler->num_threads = 0;

	return 0;
}


void *so_run(void *arg)
{
	thread_t *thread = (thread_t *)arg;

	thread->handler(thread->priority);
	thread->state = 0;
	sem_post(&thread->sem);

	return NULL;
}

tid_t so_fork(so_handler *func, unsigned int priority)
{
	// check if arguments are valid
	if (priority > SO_MAX_PRIO || !func)
		return INVALID_TID;

	// create new thread
	thread_t *thread = (thread_t *)malloc(sizeof(thread_t));

	thread->id = scheduler->num_threads;
	thread->handler = func;
	thread->priority = priority;
	thread->time_left = scheduler->time_quantum;
	thread->state = 3;
	sem_init(&thread->sem, 0, 0);

	// add thread to threads array
	scheduler->threads[scheduler->num_threads] = thread;
	scheduler->num_threads++;

	// add thread to queue
	push(scheduler->queue, thread, priority);

	// start the thread
	pthread_create(&thread->id, NULL, so_run, thread);

	return thread->id;
}

int so_wait(unsigned int io)
{
	scheduler->running->io = io;
	scheduler->running->state = 2;
	so_exec();
	return 0;
}

void so_exec(void)
{
	if (scheduler->running == NULL)
		return;
	scheduler->running->time_left--;
	schedule_task();

	sem_wait(&scheduler->running->sem);
}

void so_end(void)
{
	// check if scheduler is initialized
	if (INIT == 0)
		return;

	for (int i = 0; i < scheduler->num_threads; i++) {
		// wait for thread to finish
		pthread_join(scheduler->threads[i]->id, NULL);
		// destroy semaphore
		sem_destroy(&scheduler->threads[i]->sem);
		// free thread
		free(scheduler->threads[i]);
	}

	// free scheduler
	free(scheduler->threads);
	free(scheduler->running);
	freeQueue(scheduler->queue);
	free(scheduler);
	INIT = 0;
}

int so_signal(unsigned int io)
{
	// check if arguments are valid
	if (io >= scheduler->io || io < 0)
		return -1;

	int found = 0;
	// check if there is a thread waiting for the io
	for (int i = 0; i < scheduler->num_threads; i++) {
		if (scheduler->threads[i]->io == io && scheduler->threads[i]->state == 2) {
			scheduler->threads[i]->io = -1;
			scheduler->threads[i]->state = 0;
			scheduler->threads[i]->time_left = scheduler->time_quantum;
			push(scheduler->queue, scheduler->threads[i], scheduler->threads[i]->priority);
			found++;
		}
	}

	schedule_task();

	return found;
}

// plans the next task to be executed
void schedule_task(void)
{
	thread_t *current = scheduler->running;

	// if thread is waiting or finished
	if (current == NULL || current->state == 2 || current->state == 4) {
		thread_t *new = ((thread_t *)pop(scheduler->queue));

		new->state = 1;
		new->time_left = scheduler->time_quantum;
		sem_post(&new->sem);
		scheduler->running = new;

		return;
	}

	// if there is no other thread in the queue
	if (isEmpty(scheduler->queue)) {
		sem_post(&current->sem);

		return;
	}

	// if there is a thread with higher priority in the queue
	if (current->priority < ((thread_t *)peek(scheduler->queue))->priority) {
		thread_t *new = ((thread_t *)pop(scheduler->queue));

		current->state = 0;
		current->time_left = scheduler->time_quantum;
		sem_post(&current->sem);
		push(scheduler->queue, current, current->priority);

		new->state = 1;
		new->time_left = scheduler->time_quantum;
		sem_post(&new->sem);
		scheduler->running = new;

		return;
	}

	// if the time quantum has expired
	if (current->time_left <= 0) {
		if (current->priority == ((thread_t *)peek(scheduler->queue))->priority) {
			thread_t *new = ((thread_t *)pop(scheduler->queue));

			current->state = 0;
			current->time_left = scheduler->time_quantum;
			sem_post(&current->sem);
			push(scheduler->queue, new, new->priority);

			new->state = 1;
			new->time_left = scheduler->time_quantum;
			sem_post(&new->sem);
			scheduler->running = new;

			return;
		}
		current->time_left = scheduler->time_quantum;
	}
	sem_post(&current->sem);
}
