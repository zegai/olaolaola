#ifndef _LOOP_
#define _LOOP_

typedef struct iohandle_{
	unsigned handle_;
	buffer_queue* bq_;
	queue* msg_q_;
}iohandle;

#endif