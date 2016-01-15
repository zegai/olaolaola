#include "comm.h"
#include "loop.h"
#include "poll.h"

#define INIT_DATA_COUNT 128

int 
main_thread_loop(Poll* poll_, void* udata){
	Check(poll_, "POLL ENV NEED INIT")
	
	net_add(poll_->elfd, poll_->fd, NULL);

	sock_bind( poll_->fd, poll_->server_addr );
	sock_listen( poll_->fd );

	global_queue_init();
	for( ; ; ){
		node tmp[5];
		int count = net_wait(poll_->elfd, tmp, 5);
		if( count ){
			for(int i=0 ; i < count ; ++i){
				node* nnode = (node*)malloc(sizeof(node));
				CHECK_MEM( nnode )
				memcpy(nnode, &tmp[i], sizeof(node));
				global_queue_push( nnode );
			}
		}
	}
	global_release_queue();
}

int 
io_thread_loop(Poll* poll_, void *udata){
	//io线程负责创建buf 工作线程负责销毁
	Check(poll_, "POLL ENV NEED INIT")
	buffer_queue* b_q = buffer_queue_create();
	assert( b_q );
	for( ; ;){
		node* accept = global_queue_pop();
		if( accept ){
			node* makenode = buf_new( b_q,  INIT_DATA_COUNT);
			/*
			
			accept
			
			*/
		}
	}
}

node*
work_thread_step(iohandle* ih){
	node* p = NULL;
	for (; ;){
		p = lock_queue_pop( ih->msg_q_ );
		if( p )
			break;
	}
	return p;
}

void 
work_call_step(iohandle* ih){
	
}