#include "sock.h"


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
	return listen(sockfd, 0);
}

int 
sock_accept(int listenfd, struct sockaddr * addr){
	socklen_t socklen = sizeof(struct sockaddr_in);
	return accept(listenfd, addr, &socklen);
}

inline int 
sock_send(){

}

inline int 
sock_recv(){

}

int 
sock_release(){
	
}
