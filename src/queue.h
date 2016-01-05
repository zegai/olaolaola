#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "comm.h"

typedef struct queue_node_{
	node* data;
	struct queue_node_* next;
}queue_node;

typedef struct queue_{
	queue_node* head;
	queue_node* tail;
	int size;
	LockType lock_;
}queue;


queue* (queue_init)();
void (queue_push)(queue* mqueue_, node* nnode);
node* (queue_pop)(queue* mqueue_);
void (release_queue)();
void (global_queue_init)();
void (global_queue_push)(node* nnode);
node* (global_queue_pop)();

#endif
