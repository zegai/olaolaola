#include "comm.h"
#include "loop.h"
#include "poll.h"

#define INIT_DATA_COUNT 128
#define BUSY_BIT_SIZE	4

typedef struct listen_{
	int lsfd;
	int elfd;
	int link_count;
}listen_handle;


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
				makenode->session = sock_accept(udata->lsfd);
				net_add(udata->elfd, makenode->session, makenode);
				udata->link_count ++;
				break;
			case SOCK_READ:
				int read_count = read(accept->session, accept->udata, accept->size);
				if(read_count == accept->size){
					accept = buf_reverse(b_q, accept, accept->size*2 );
				}
				break;
			case SOCK_WRITE:
				break;
			case SOCK_RBUF:
				//release buf
				buf_release( b_q , accept);
				break;
			case SOCK_RACCEPT:

				break;
			case SOCK_CLOSE:
				close(accept->session);
			default:
				break;
		}

	}
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
	listen_handle* listen_data = (listen_handle* )malloc(sizeof(listen_handle))
	iohandle* io = (iohandle *)malloc(sizeof(iohandle*)*lg->io_thread_count_);
	CHECK_MEM( listen_data && io)
	
	listen_data->lsfd = poll_->fd;
	listen_data->elfd = poll_->elfd;
	listen_data->link_count = 0;
	listennode.data_type = SOCK_ACCEPT;
	listennode.udata = listen_data;
	
	reacceptnode.data_type = SOCK_RACCEPT;
	reacceptnode.data_value = 0;
	net_add(poll_->elfd, poll_->fd, &listennode);


	sock_bind( poll_->fd, poll_->server_addr );
	sock_listen( poll_->fd );
	
	global_queue_init();
	for( ; ; ){	
		node* tmp[5];
		int count = net_wait(poll_->elfd, tmp[0], 5);
		if( count ){
			for(int i=0 ; i < count ; ++i){
				if(tmp[i]->data_type == SOCK_ACCEPT){
					lock_queue_push( io[accept_val++],tmp[i] );
					accept_val %= thread_count;
				}
				else{
					if(io[tmp[i]->data_value]->busy){
						/**负载---未完成**/
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
					else{
						lock_queue_push(io[tmp[i]->data_value]->msg_q_, tmp[i]);
					}
				}
			}
		}
		if( poll_->quit_flg )
			break;
	}
quit:
	global_release_queue();
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
work_thread_get_step(iohandle* ih){
	node* p = lock_queue_pop( ih->msg_q_ );
	return p;
}


void 
work_call_step(int handle, iohandle* ih){
	
}