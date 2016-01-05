#include "sock.h"
#include "comm.h"

int 
sock_create(){
	return socket(AF_UNIX, SOCK_STREAM, 0);
}

int 
sock_bind(int sockfd, struct sockaddr_in* addr){
	assert( sockfd && addr );
	return bind(sockfd, (struct sockaddr *)addr, sizeof(struct sockaddr_in));
}

int 
sock_listen(int sockfd){
	return listen(sockfd, LISTEN_BACKLOG);
}


int 
sock_send(){

}

int 
sock_recv(){

}

int 
sock_release(){
	
}
