#include "buffer.h"
#include <string.h>

#define SMALL_BUF 	3 // < 64 byte small
#define MIDDLE_BUF 	6 // 64 - 512 byte
#define BIG_BUF 	10	// 512 - 1024

#define MAX_SBUF	64
#define MAX_MBUF	512
#define MAX_BBUF	1024

buffer_queue* 
buffer_queue_create(int buf_size){
	buffer_queue* bq_ = NULL;
	bq_ = (buffer_queue* )malloc(sizeof(buffer_queue));
	CHECK_MEM(bq_);
	bq_->s_q = queue_init();
	bq_->m_q = queue_init();
	bq_->b_q = queue_init();
	bq_->buf_size = buf_size;
	bq_->small_count = 0;
	bq_->small_full_count = MAX_MBUF / MAX_SBUF;
	bq_->full_buf = malloc(buf_size);
	bq_->big_buf = malloc(buf_size);
	CHECK_MEM(bq_->s_q && bq_->m_q && bq_->b_q);
	bq_->small_pos = bq_->full_buf;
	bq_->middle_pos = bq_->full_buf + buf_size;
	bq_->big_pos = bq_->big_buf;
	return bq_;
}


static node*
buf_create_small(buffer_queue* queue_, int size){
	int small_count,small_full_count;
	small_count = queue_->small_count;
	small_full_count = queue_->small_full_count;
	if( small_count < small_full_count ){
		AtomInc(small_count);
	}else{
		if(queue_->small_pos != queue_->middle_pos){
			AtomExc(queue_->small_pos, MAX_MBUF);
			///errrrrrr
			queue_->small_count = 0;
		}else{
			Check(-1, "out of buf");
		}
	}
	node* nnode = (node* )(queue_->small_pos - MAX_SBUF*small_count );
	nnode->info.size = size;
	nnode->info.cursize = 0;
	nnode->udata = nnode + sizeof(nnode);
	return nnode;
}

static node*
buf_create_middle(buffer_queue* queue_, int size){
	int small_count,small_full_count;
	small_count = queue_->small_count;
	small_full_count = queue_->small_full_count;
	if(queue_->small_pos != queue_->middle_pos){
		queue_->middle_pos -= MAX_MBUF;
	}else{
		Check(-1, "out of buf");
	}
	node* nnode = (node* )( queue_->middle_pos );
	nnode->info.size = size;
	nnode->info.cursize = 0;
	nnode->udata = nnode + sizeof(node);
	return nnode;
}

static node*
buf_create_big(buffer_queue* queue_, int size){
	node* nnode = (node *)( queue_->big_pos );
	printf("1");
	queue_->big_pos += sizeof(node) + size;
	printf("2");
	nnode->info.size = size;
	nnode->info.cursize = 0;
	nnode->udata = nnode + sizeof(node);
	return nnode;
}

static node*
buf_create_max(int size){
	node* nnode = (node *)malloc(sizeof(node) + size);
	CHECK_MEM(nnode);
	nnode->info.size = size;
	nnode->info.cursize = 0;
	nnode->udata = nnode + sizeof(node);
	return nnode;
}

node*
buf_new( buffer_queue* queue_, int size ){
	assert(size);
	int fullsize = size + sizeof(node);
	if( !(fullsize >> SMALL_BUF)){
		if( queue_->s_q->size ){
			node* buf_ = queue_pop(queue_->s_q);
			buf_->info.cursize = 0;
			return buf_;
		}else{
			return buf_create_small(queue_, size);
		}
	}
	else if( !(fullsize >> MIDDLE_BUF) ){
		if ( queue_->m_q->size ){
			node* buf_ = queue_pop(queue_->m_q);
			buf_->info.cursize = 0;
			return buf_;
		}else{
			return buf_create_middle(queue_, size);
		}
	}
	else if( !(fullsize >> BIG_BUF) ){
		if ( queue_->b_q->size ){
			node* buf_ = queue_pop(queue_->b_q);
			buf_->info.cursize = 0;
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
	int size = nnode->info.size;
	nnode->info.cursize = 0;
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
	p->info.cursize = size;
	buf_release(bq_, curnode);
	return p;
}

int 
buffer_check(buffer_queue* bq_){
	
}


