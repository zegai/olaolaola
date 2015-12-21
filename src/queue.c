#include "comm.h"
#include "queue.h"
#include <string.h>
#define OPENAPI

typedef struct queue_node_{
	node* data;
	struct queue_node_* next;
}queue_node;

typedef struct queue_{
	queue_node* head;
	queue_node* tail;
	int size;
	LockType lock_;
}queue;


static queue* msg_q = NULL;
static queue* buf_q = NULL;


OPENAPI void 
queue_init(){
	assert( msg_q == NULL );
	msg_q = (queue *)malloc(sizeof(queue));
	buf_q = (queue *)malloc(sizeof(queue));
	CHECK_MEM(msg_q && buf_q);
	memset(msg_q, 0, sizeof(queue));
	memset(buf_q, 0, sizeof(queue));
}

static queue_node*
make_queue_node(queue* mqueue_){
	assert( mqueue_ );

	queue_node* p = mqueue_->head;
	if( p ){
		mqueue_->head = p->next;
		p->next = NULL;
		mqueue_->size --;
		return p;
	}
	p = (queue_node*)malloc(sizeof(queue_node));
	CHECK_MEM(p);
	return p;
}



static node*
release_queue_node(queue* mqueue_, queue_node* p){
	assert( mqueue_ && p );

	if( mqueue_->size > MAX_BUF_Q ){
		node* r = p->data;
		free( p );
		return r;
	}
	p->data = NULL;
	p->next = NULL;
	if( !mqueue_->head ){
		mqueue_->head = p;
	}
	else if( !mqueue_->tail ){
		mqueue_->head->next = p;
		mqueue_->tail = p;
	}
	else{
		mqueue_->tail->next = p;
		mqueue_->tail = p;
	}
	mqueue_->size ++;
	return p->data;
}


OPENAPI void
queue_insert(node* nnode){
	assert( msg_q );
	queue* mqueue_ = msg_q;

	Lock( mqueue_->lock_ );
	queue_node *p = make_queue_node(mqueue_);
	if( !mqueue_->head ){
		mqueue_->head = p;
	}
	else if( !mqueue_->tail ){
		mqueue_->head->next = p;
		mqueue_->tail = p;
	}
	else{
		mqueue_->tail->next = p;
		mqueue_->tail = p;
	}
	UnLock(mqueue_->lock_);
}

OPENAPI node*
queue_pop(){
	assert( msg_q );
	node* p = NULL;
	queue* mqueue_ = msg_q;
	queue* bqueue_ = buf_q;
	Lock( mqueue_->lock_ );
	if( mqueue_->head ){
		queue_node* q = mqueue_->head;
		mqueue_->head = q->next;
		p = release_queue_node(bqueue_, q);
	}
	UnLock( mqueue_->lock_ );
	return p;
}

OPENAPI int 
queue_len(){
	return msg_q->size;
}

static void
release_buf_queue(){
	assert( buf_q );
	int i = 0;
	int size = buf_q->size;
	for(; i < size ; i++){
		free(make_queue_node(msg_q));
	}

}

OPENAPI void
release_queue(){
	assert( msg_q && buf_q );
	node* p = queue_pop();
	while( p ){
		//node_call_(p);
		p = queue_pop();
	}
	release_buf_queue();

	free(msg_q);
	free(buf_q);

}



