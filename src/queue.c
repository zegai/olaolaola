#include "comm.h"
#include "queue.h"
#include <string.h>
#define OPENAPI




static queue* g_msg_q = NULL;
static queue* g_buf_q = NULL;


OPENAPI void 
global_queue_init(){
	assert( g_msg_q == NULL );
	g_msg_q = (queue *)malloc(sizeof(queue));
	g_buf_q = (queue *)malloc(sizeof(queue));
	CHECK_MEM(g_msg_q && g_buf_q);
	memset(g_msg_q, 0, sizeof(queue));
	memset(g_buf_q, 0, sizeof(queue));
}

queue*
queue_init(){
	queue* q_ = NULL;
	q_ = (queue *)malloc(sizeof(queue));
	CHECK_MEM(q_);
	memset(q_, 0, sizeof(queue));
	return q_;
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
queue_pop(queue* mqueue_){
	assert( mqueue_ );
	node* p = NULL;
	queue* bqueue_ = g_buf_q;
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
	return g_msg_q->size;
}

static void
release_buf_queue(){
	assert( g_buf_q );
	int i = 0;
	int size = g_buf_q->size;
	for(; i < size ; i++){
		free(make_queue_node(g_buf_q));
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
	return queue_pop(g_msg_q);
}

OPENAPI void
global_queue_push(node* nnode){
	assert( g_msg_q );
	queue* mqueue_ = g_msg_q;
	queue_push(mqueue_, nnode);
}
