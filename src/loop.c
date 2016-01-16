#include "comm.h"
#include "loop.h"
#include "poll.h"

#define INIT_DATA_COUNT 128

typedef struct listen_{
	int lsfd;
	int elfd;
	int link_count;
}listen_handle;

int 
main_thread_loop(Poll* poll_, void* udata){
restart:
	Check(poll_, "POLL ENV NEED INIT")
	node* listennode = (node*)malloc(sizeof(node));
	listen_handle* listen_data = (listen_handle* )malloc(sizeof(listen_handle))
	CHECK_MEM( listennode && listen_data )
	
	listen_data->lsfd = poll_->fd;
	listen_data->elfd = poll_->elfd;
	listen_data->link_count = 0;
	listennode->data_type = SOCK_ACCEPT;
	listennode->udata = listen_data;
	
	net_add(poll_->elfd, poll_->fd, listennode);

	sock_bind( poll_->fd, poll_->server_addr );
	sock_listen( poll_->fd );

	global_queue_init();
	for( ; ; ){
		node* tmp[5];
		int count = net_wait(poll_->elfd, tmp[0], 5);
		if( count ){
			for(int i=0 ; i < count ; ++i){
				global_queue_push( tmp[i] );
			}
		}
		if( poll_->quit_flg )
			break;
	}
quit:
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
		switch( accept->data_type ){
			case SOCK_ACCEPT:
				node* makenode = buf_new( b_q,  INIT_DATA_COUNT);
				printf("New Connect Linked\n");
				listen_handle* udata = (listen_handle *)accept->udata;
				makenode->val.session = sock_accept(udata->lsfd);
				net_add(accept->elfd, makenode->val.session, makenode);
				AtomInc(accept->link_count);
				break;
			case SOCK_READ:
				break;
			case SOCK_WRITE:
				break;
			case SOCK_TIMEOUT:
			case SOCK_BREAK:
				
			default:
				break;
		}

	}
}

//返回给反序列化代码
node*
work_thread_get_step(iohandle* ih){
	node* p = NULL;
	for (; ;){
		p = lock_queue_pop( ih->msg_q_ );
		if( p )
			break;
	}
	return p;
}




void 
work_call_step(int handle, iohandle* ih){
	
}