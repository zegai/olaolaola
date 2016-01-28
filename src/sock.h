#ifndef _OLA_SOCK_
#define _OLA_SOCK_
#include "comm.h"

int (sock_create)();
int (sock_bind)(int sockfd, struct sockaddr_in* addr);
int (sock_listen)(int sockfd);
int (sock_accept)(int listenfd, struct sockaddr * addr);
#endif