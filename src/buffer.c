#include "buffer.h"
#include "comm.h"
#include "queue.h"
#include <string.h>

#define SMALL_BUF 	3 // < 64 byte small
#define MIDDLE_BUF 	6 // 64 - 512 byte
#define BIG_BUF 	10	// 512 - 1024

#define MAX_SBUF	64
#define MAX_MBUF	512
#define MAX_BBUF	1024


struct bq
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

buffer_queue* 
buffer_queue_create(int buf_size){
	buffer_queue* bq_ = NULL;
	bq_ = (buffer_queue* )malloc(sizeof(buffer_queue));
	CHECK_MEM(bq_);
	bq_->small_buf = queue_init();
	bq_->big_buf = queue_init();
	bq_->buf_size = buf_size;
	bq_->small_count = 0;
	bq_->small_full_count = MAX_MBUF / MAX_SBUF;
	bq_->s_q = malloc(buf_size);
	bq_->m_q = malloc(buf_size);
	bq_->b_q = malloc(buf_size);
	CHECK_MEM(s_q && m_q && b_q);
	return bq_;
}


static node*
buf_create_small(buffer_queue* queue_, int size){
	if( small_count < small_full_count ){
		AtomInc(small_count);
	}else{
		if(small_pos != middle_pos){
			AtomExc(small_pos, MAX_MBUF);
			///errrrrrr
			small_count = 0;
		}else{
			Check(-1, "out of buf");
		}
	}
	node* nnode = (node* )(small_pos - MAX_SBUF*small_count );
	nnode->val.bufinfo.size = size;
	nnode->val.bufinfo.cursize = 0;
	nnode->udata = nnode + sizeof(nnode);
	return nnode;
}

static node*
buf_create_middle(buffer_queue* queue_, int size){
	if(small_pos != middle_pos){
		middle_pos -= MAX_MBUF;
	}else{
		Check(-1, "out of buf");
	}
	node* nnode = (node* )( middle_pos );
	nnode->val.bufinfo.size = size;
	nnode->val.bufinfo.cursize = 0;
	nnode->udata = nnode + sizeof(node);
	return nnode;
}

static node*
buf_create_big(buffer_queue* queue_, int size){
	node* nnode = (node *)( big_pos );
	big_pos += sizeof(node) + size;
	nnode->val.bufinfo.size = size;
	nnode->val.bufinfo.cursize = 0;
	nnode->udata = nnode + sizeof(node);
	return nnode;
}

static node*
buf_create_max(int size){
	nnode* nnode = (nnode *)malloc(sizeof(nnode + size));
	CHECK_MEM(nnode);
	nnode->val.bufinfo.size = size;
	nnode->val.bufinfo.cursize = 0;
	nnode->udata = nnode + sizeof(node);
	return nnode;
}

node*
buf_new( buffer_queue* queue_, int size ){
	assert(size);
	int fullsize = size + sizeof(node);
	if( !(fullsize >> SMALL_BUF)){
		if( queue_->full_buf->size ){
			node* buf_ = queue_pop(queue_->s_q);
			buf_->val.bufinfo.cursize = 0;
			return buf_;
		}else{
			return buf_create_small(queue_, size);
		}
	}
	else if( !(fullsize >> MIDDLE_BUF) ){
		if ( queue_->full_buf->size ){
			node* buf_ = queue_pop(queue_->m_q);
			buf_->val.bufinfo.cursize = 0;
			return buf_;
		}else{
			return buf_create_middle(queue_, size);
		}
	}
	else if( !(fullsize >> BIG_BUF) ){
		if ( queue_->big_buf->size ){
			node* buf_ = queue_pop(queue_->b_q);
			buf_->val.bufinfo.cursize = 0;
			return buf_;
		}else{
			return buf_create_big(queue_, size);
		}
	}else{
		return buf_create_max(size);
	}
}

void
buf_release(buffer_queue* queue_, node* nnode){
	int size = nnode->val.bufinfo.size;
	nnode->val.bufinfo.cursize = 0;
	queue* push_q = NULL;
	if( !(size >> SMALL_BUF) ){
		push_q = queue_->s_q;
	}else if( !(size >> MIDDLE_BUF) ){
		push_q = queue_->m_q;
	}else if( !(size >> BIG_BUF) ){
		push_q = queue_->b_q;
	}

	if ( push_q ){
		queue_push(push_q, nnode);
	}else{
		free( nnode->udata );
		free( nnode );
	}

}

node* 
buf_reverse(buffer_queue* bq_, node* curnode, int size){
	node* p = buf_new(bq_, size);
	memcmp( curnode->udata, p->udata, size );
	p->val.bufinfo.cursize = size;
	buf_release(bq_, curnode);
	return p;
}

int 
buffer_check(buffer_queue* bq_){
	
}


