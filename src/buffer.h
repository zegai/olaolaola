#ifndef _OLA_BUFFER_
#define _OLA_BUFFER_
#include "comm.h"
#include "queue.h"

typedef struct bq
{
	size_t buf_size;
	
	int small_count;
	int small_full_count;
	void* small_pos;
	void* middle_pos;
	void* big_pos;
	void* full_buf;
	void* big_buf;

	queue* s_q;
	queue* m_q;
	queue* b_q;
}buffer_queue;

buffer_queue* (buffer_queue_create)(int buf_size);
node* (buf_new)(buffer_queue* bq, int size);
void (buf_release)(buffer_queue* queue_, node* nnode);
node* (buf_reverse)(buffer_queue* bq, node* buf, int size);

#endif
