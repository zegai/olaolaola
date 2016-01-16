#include "comm.h"
#include "queue.h"
#include <string.h>
#define OPENAPI



static queue* g_msg_q = NULL;

queue*
queue_init(){
	queue* q_ = NULL;
	q_ = (queue *)malloc(sizeof(queue));
	CHECK_MEM(q_);
	memset(q_, 0, sizeof(queue));
	q_->tmp_queue_ = (queue_ *)malloc(sizeof(queue));
	CHECK_MEM( q_->tmp_queue_ );
	return q_;
}

OPENAPI void 
global_queue_init(){
	assert( g_msg_q == NULL );
	g_msg_q = queue_init();
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
queue_push(queue* mqueue_, node* nnode){

	queue_node *p = make_queue_node(mqueue_->tmp_queue_);
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
	
}

OPENAPI node*
queue_pop(queue* mqueue_){
	assert( mqueue_ );
	node* p = NULL;
	
	if( mqueue_->head ){
		queue_node* q = mqueue_->head;
		mqueue_->head = q->next;
		p = release_queue_node(mqueue_->tmp_queue_, q);
	}
	return p;
}

OPENAPI int 
queue_len(){
	return g_msg_q->size;
}

static void
release_buf_queue(queue_* mqueue_){
	
	int i = 0;
	int size = mqueue_->tmp_queue_->size;
	for(; i < size ; i++){
		free(make_queue_node(mqueue_->tmp_queue_));
	}

}

OPENAPI void
release_queue(queue* msg_q){
	assert( msg_q );
	node* p = queue_pop();
	while( p ){
		free(p->udata);
		free(p);
		p = queue_pop(msg_q);
	}

	free(msg_q);
}

OPENAPI void
global_release_queue(){
	release_queue(g_msg_q);
}

OPENAPI node*
global_queue_pop(){
	Lock( g_msg_q->lock_ );
	node* p = queue_pop(g_msg_q);
	UnLock( g_msg_q->lock_ );
	return p;
}

OPENAPI void
global_queue_push(node* nnode){
	assert( g_msg_q );
	queue* mqueue_ = g_msg_q;
	Lock( mqueue_->lock_ );
	queue_push(mqueue_, nnode);
	UnLock(mqueue_->lock_);
}

OPENAPI void 
lock_queue_push(queue* msg_q, node* nnode){
	assert( msg_q && nnode );
	queue* mqueue_ = msg_q;
	Lock( mqueue_->lock_ );
	queue_push(mqueue_, nnode);
	UnLock(mqueue_->lock_);
}

OPENAPI node* 
lock_queue_pop(queue* msg_q){
	Lock( msg_q->lock_ );
	node* p = queue_pop(msg_q);
	UnLock( msg_q->lock_ );
	return p;
}




