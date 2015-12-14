#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "comm.h"

void (queue_init)();
void (set_release_node_call)(reg_release_callback);
void (queue_insert)(node*);
node* (queue_pop)();
void (release_queue)();


#endif