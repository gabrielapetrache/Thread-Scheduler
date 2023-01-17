/**
* Priority Queue implementation using a linked list
*/

typedef struct node {
	void *data;
	int priority;
	struct node *next;
} Node;

typedef struct priortiy_queue {
	Node *head;
	int size;
} PriorityQueue;

// creates a new priority queue
PriorityQueue *createPriorityQueue(void);

// creates a new node
Node *newNode(void *data, int priority);

// pushes a node into the queue
void push(PriorityQueue *queue, void *data, int priority);

// pops the node with the highest priority
void *pop(PriorityQueue *queue);

// returns the node with the highest priority
void *peek(PriorityQueue *queue);

// returns 1 if the queue is empty, 0 otherwise
int isEmpty(PriorityQueue *queue);

// returns the size of the queue
int size(PriorityQueue *queue);

// frees the queue
void freeQueue(PriorityQueue *queue);
