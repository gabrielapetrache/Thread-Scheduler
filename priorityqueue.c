#include <stdio.h>
#include <stdlib.h>
#include "priorityqueue.h"

PriorityQueue *createPriorityQueue(void)
{
	PriorityQueue *queue = (PriorityQueue *)malloc(sizeof(PriorityQueue));

	queue->head = NULL;
	queue->size = 0;

	return queue;
}

Node *newNode(void *data, int priority)
{
	Node *node = (Node *)malloc(sizeof(Node));

	node->data = data;
	node->priority = priority;
	node->next = NULL;

	return node;
}

void push(PriorityQueue *queue, void *data, int priority)
{
	Node *start = queue->head;

	Node *node = newNode(data, priority);

	queue->size++;
	if (queue->head == NULL) {
		queue->head = node;
		return;
	}
	if (queue->head->priority > priority) {
		node->next = queue->head;
		queue->head = node;
		return;
	}
	while (start->next != NULL && start->next->priority < priority)
		start = start->next;
	node->next = start->next;
	start->next = node;
}

void *pop(PriorityQueue *queue)
{
	if (queue->head == NULL)
		return NULL;
	Node *temp = queue->head;

	queue->head = queue->head->next;
	void *data = temp->data;

	free(temp);
	queue->size--;

	return data;
}

void *peek(PriorityQueue *queue)
{
	if (queue->head == NULL)
		return NULL;

	return queue->head->data;
}

int isEmpty(PriorityQueue *queue)
{
	return queue->size == 0;
}

int size(PriorityQueue *queue)
{
	return queue->size;
}

void freeQueue(PriorityQueue *queue)
{
	Node *temp = queue->head;

	while (temp != NULL) {
		Node *next = temp->next;
		free(temp);
		temp = next;
	}
	free(queue);
}
