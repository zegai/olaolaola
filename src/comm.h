#ifndef _COMM_H_
#define _COMM_H_
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <netinet/in.h>
#include <pthread.h>
#ifdef WIN32
#define LockType CRITICAL_SECTION
#define InitSpinLock(p, count) InitializeCriticalSectionAndSpinCount(&p , count)  
#define InitLock(p) InitializeCriticalSection(&p)

#define Lock(p) EnterCriticalSection(&p)
#define UnLock(p) LeaveCriticalSection(&p)

#define AtomInc(p) ::InterlockedIncrement(&p) //p++ AtomInc(p)
#define AtomDec(p) ::InterlockedDecrement(&p) //p--	AtomDec(p)
#define AtomExc(p, count) ::InterlockedExchangeAdd(&p, count) // p += count AtomExc(p, count)
#define AtomCas()

#else

#define LockInit(p) pthread_mutex_init(&p, NULL)
#define Lock(p) pthread_mutex_lock(&p)
#define UnLock(p) pthread_mutex_unlock(&p)
#define LockRelease(p) pthread_mutex_destroy(&p)
#define LockType pthread_mutex_t
#define AtomInc(ptr) __sync_add_and_fetch(&ptr, 1)
#define AtomExc(ptr, count) __sync_add_and_fetch(&ptr, count)
#define AtomCas(ptr, oldval, newval) __sync_bool_compare_and_swap(ptr, oldval, newval)
#define AtomCasv(ptr, oldval, newval) __sync_val_compare_and_swap(ptr, oldval, newval)
#endif


#define Check(p, str) if(!(p)){printf(str);}
#define CHECK_MEM(p) Check(p,"OUT MEMMORY\n")

#define comm_u32 unsigned
#define COMM_API
#define MAX_BUF_Q 1024

#define SOCK_ACCEPT 	0
#define SOCK_READ	1
#define SOCK_WRITE	2
#define SOCK_RBUF	3
#define SOCK_REACCEPT	4
#define SOCK_CLOSE	5

typedef struct Poll_{
	comm_u32  fd;
	comm_u32  elfd;
	struct sockaddr_in server_addr;
	LockType global_lock_;
	int quit_flg;
}Poll;

struct bufinfo
{
	void* bq;
	int size;
	int cursize;
};

typedef struct data_node_{
	int session;
	struct bufinfo info;
	short data_type;
	short data_value;
	void* udata;
}node;

typedef struct logi_Set_{
	const char* call_func_name_;
	const char* call_file_path_;
	const char* worker_name_;
	const char* worker_path_;
	const char* global_value_;
	const char* log_path_;
	int io_thread_count_;
	int thread_count_;
	int port_;
	int Mtu_;

}lg_set;


COMM_API const char* (init_poll_env)(const char* config_path_);
COMM_API int (init_worker_env)();
COMM_API void (release_worker_env)();
#endif
