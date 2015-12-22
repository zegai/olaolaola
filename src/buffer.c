#include "buffer.h"
#include "comm.h"
#include <string.h>

#define SMALL_BUF 64 // < 64 byte small
#define MIDDLE_BUF 512 // 64 - 512 byte
#define BIG_BUF 1024	// 512 - 1024

#define MAX_SBUF	1024
#define MAX_MBUF	256
#define MAX_BBUF	64


buff*
buf_new( int size ){
	assert(size);
	
}
