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
main_thread_loop(Poll* poll_, void* udata){
restart:
	Check(poll_, "POLL ENV NEED INIT")
	node* listennode = (node*)malloc(sizeof(node));
	node* reacceptnode = (node*)malloc(sizeof(node));
	lg_set* lg = (lg_set*)udata;
	int accept_val = 0;
	int thread_count = lg->io_thread_count_;
	listen_handle* listen_data = (listen_handle* )malloc(sizeof(listen_handle))
	iohandle* io = (iohandle *)malloc(sizeof(iohandle*)*lg->io_thread_count_);
	CHECK_MEM( listennode && listen_data && reacceptnode && io)
	
	listen_data->lsfd = poll_->fd;
	listen_data->elfd = poll_->elfd;
	listen_data->link_count = 0;
	listennode->data_type = SOCK_ACCEPT;
	listennode->udata = listen_data;
	
	reacceptnode->data_type = SOCK_REACCEPT;

	net_add(poll_->elfd, poll_->fd, listennode);


	sock_bind( poll_->fd, poll_->server_addr );
	sock_listen( poll_->fd );
	/************** INIT IO THREAD ***************
	//
	//
	//
	//
	*********************************************/
	
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
					/*需要负载均衡*/
					int findflg = -1;
					if(io[tmp[i]->io_count]->msg_q_->size >> BUSY_BIT_SIZE){
						//对应线程中队列过长
						for(int search = 0; search < thread_count ; search++ ){
							if(io[search]->msg_q_->size >> BUSY_BIT_SIZE)
								continue;
							else{
								findflg = search;
								break;
							}
						}
						if(findflg != -1){
						}
						else{

						}
						
					}else{
						lock_queue_push(io[tmp[i]->io_count]->msg_q_, tmp[i]);
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
				net_add(udata->elfd, makenode->val.session, makenode);
				udata->link_count ++;
				break;
			case SOCK_READ:
				int read_count = read(accept->val.session, accept->udata, accept->size);
				if(read_count == accept->size){
					accept = buf_reverse(b_q, accept, accept->size*2 );
				}
				break;
			case SOCK_WRITE:
				write(accept->val.session, accept->udata, accept->val.cursize);
				break;
			case SOCK_REACCEPT:

				break;
			case SOCK_TIMEOUT:
			case SOCK_CLOSE:
				close(accept->val.session);
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