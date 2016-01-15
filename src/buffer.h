#ifndef _OLA_BUFFER_
#define _OLA_BUFFER_
#include "comm.h"

buffer_queue* (buffer_queue_create)()
node* (buf_new)(buffer_queue* bq, int size);
int (buf_set)(node* buf, void* data, int size);
int (buf_release)(node* buf);
node* (buf_reverse)(buffer_queue* bq, node* buf, int size);
void (buf_queue_release)(buffer_queue* bq)

#endif