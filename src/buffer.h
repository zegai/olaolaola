#ifndef _OLA_BUFFER_
#define _OLA_BUFFER_

typedef struct{
	int size;
	char p[];
}buff;

buff* (buf_new)(int size);
int (buf_set)(buff* buf, void* data, int size);
int (buf_release)(buff* buf);
buff* (buf_reverse)(buff* buf);

#endif