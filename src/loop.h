#ifndef _LOOP_
#define _LOOP_
#include "comm.h"
#include "buffer.h"

typedef struct iohandle_{
	unsigned handle_;
	buffer_queue* bq_;
	queue* msg_q_;
}iohandle;

#endif
