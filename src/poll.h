#ifndef _NET_H_
#define _NET_H_
#include "comm.h"
static int
net_init(){
	return epoll_create(1024);	
}

static int 
net_add(int epfd, int sockfd, void* ptr){
	CHECK(ptr, "GET NULL PTR")
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = ptr;
	CHECK((epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) == 0), "Err When Add Epoll Event")
	return 1;
}

static int 
net_del(int epfd, int sockfd){
	epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
	return 1;
}

static int
net_wait(int epfd, node* retnode, int waitsize){
	assert( waitsize );
	struct epoll_event ev[waitsize];
	int size = epoll_wait(efd, ev, waitsize, -1);
	int i = 0;
	for (; i < size; ++i){
		retnode[i].udata = ev[i].data.ptr;
	}
	return size;
}

#endif