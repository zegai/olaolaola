#ifndef _LOOP_
#define _LOOP_

typedef void (extend_func)(node*);

typedef struct iohandle_{
	unsigned handle_;
	buffer_queue* bq_;
	queue* msg_q_;
	int busy;
}iohandle;

#endif