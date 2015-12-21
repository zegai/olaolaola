#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "comm.h"

void (queue_init)();
void (queue_insert)(node*);
node* (queue_pop)();
void (release_queue)();


#endif
