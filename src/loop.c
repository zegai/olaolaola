#include "loop.h"
#include "poll.h"
#include "sock.h"
#include <pthread.h>
#define INIT_DATA_COUNT 128
#define BUSY_BIT_SIZE	4

typedef struct listen_{
	int lsfd;
	int elfd;
	int link_count;
}listen_handle;

typedef int (*pwork)(buffer_queue* ,node*);

int 
io_accept(buffer_queue* b_q, node* pnode){
	node* makenode = buf_new( b_q,  INIT_DATA_COUNT);
	printf("New Connect Linked\n");
	listen_handle* udata = (listen_handle *)pnode->udata;
	struct sockaddr addr;
	makenode->session = sock_accept(udata->lsfd, &addr);
	net_add(udata->elfd, makenode->session, makenode);
	udata->link_count ++;
}

int 
io_read(buffer_queue* b_q, node* pnode){
	int read_count = read(pnode->session, pnode->udata, pnode->info.size);
	if(read_count == pnode->info.size){
		printf("out buffer\n");
		pnode = buf_reverse(b_q, pnode, pnode->info.size*2 );
	}
	char* value = ((char*)(pnode->udata) + read_count + 1);
	printf("receive from buf : %s\n", pnode->udata);
}

int 
io_write(buffer_queue* b_q, node* pnode){

}

int 
io_rbuf(buffer_queue* b_q, node* pnode){
	buf_release( b_q , pnode);
}

int 
io_reaccept(buffer_queue* b_q, node* pnode){

}

int 
io_close(buffer_queue* b_q, node* pnode){
	close(pnode->session);
}

void* 
io_thread_loop(void *udata){
	assert( udata );
	iohandle* pio = (iohandle*)udata;
	buffer_queue* b_q = pio->bq_;
	
	pwork pdeal[] = {
		&io_accept,
		&io_read,
		&io_write,
		&io_rbuf,
		&io_reaccept,
		&io_close
	};
	
	for( ; ;){
		node* accept = lock_queue_pop(pio->msg_q_);
		if(accept){
			pdeal[accept->data_type](b_q ,accept);
		}
	}
	return udata;
}

int 
init_io(iohandle* io){
	pthread_t ppid;
	pthread_create(&ppid, NULL, &io_thread_loop, io);
}

int 
main_thread_loop(Poll* poll_, void* udata){
restart:
	Check(poll_, "POLL ENV NEED INIT")
	node listennode;
	node reacceptnode;
	lg_set* lg = (lg_set*)udata;
	int accept_val = 0;
	int thread_count = lg->io_thread_count_;
	listen_handle* listen_data = (listen_handle* )malloc(sizeof(listen_handle));
	iohandle* io = (iohandle *)malloc(sizeof(iohandle)*lg->io_thread_count_);
	CHECK_MEM( listen_data && io)
	
	listen_data->lsfd = poll_->fd;
	listen_data->elfd = poll_->elfd;
	listen_data->link_count = 0;
	listennode.data_type = SOCK_ACCEPT;
	listennode.udata = listen_data;
	
	reacceptnode.data_type = SOCK_REACCEPT;
	reacceptnode.data_value = 0;
	net_add(poll_->elfd, poll_->fd, &listennode);
	init_io(io);

	sock_bind( poll_->fd, &poll_->server_addr );
	sock_listen( poll_->fd );
	
	//global_queue_init();
	for( ; ; ){	
		node* tmp[5];
		int count = net_wait(poll_->elfd, tmp[0], 5);
		if( count ){
			for(int i=0 ; i < count ; ++i){
				if(tmp[i]->data_type == SOCK_ACCEPT){
					lock_queue_push( io->msg_q_,tmp[i] );
					accept_val %= thread_count;
				}
				else{
					/**负载
					if(io[tmp[i]->data_value]->busy){
						
						if(!reacceptnode.data_value){
							for(int search = 0; search < thread_count ; search ++){
								if(io[search]->busy)
									continue;
								else{
									reacceptnode.session = tmp[i]->session;
									reacceptnode.info.size = tmp[i]->info.size;
									tmp[i]->data_type = SOCK_RBUF;
									lock_queue_push(io[tmp[i]->data_value]->msg_q_, tmp[i]);
									lock_queue_push(io[search]->msg_q_, &reacceptnode);
								}
							}
						}
					}
					---未完成**/
					lock_queue_push(io->msg_q_, tmp[i]);
				}
			}
		}
		if( poll_->quit_flg )
			break;
	}
	//global_release_queue();
	return 1;
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

node*
work_thread_get_a_step(iohandle* ih){
	node* p = lock_queue_pop( ih->msg_q_ );
	return p;
}


void 
work_call_step(int handle, iohandle* ih){
	
}
