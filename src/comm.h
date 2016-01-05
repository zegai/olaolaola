#ifndef _COMM_H_
#define _COMM_H_
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#ifdef WIN32

#define LockType CRITICAL_SECTION
#define InitSpinLock(p, count) InitializeCriticalSectionAndSpinCount(&p , count)  
#define InitLock(p) InitializeCriticalSection(&p)
#ifdef NEED_LOCK
#define Lock(p) EnterCriticalSection(&p)
#define UnLock(p) LeaveCriticalSection(&p)
#else
#define Lock(p) 
#define UnLock(p) 
#endif
#define AtomInc(p) ::InterlockedIncrement(&p) //p++ AtomInc(p)
#define AtomDec(p) ::InterlockedDecrement(&p) //p--	AtomDec(p)
#define AtomExc(p, count) ::InterlockedExchangeAdd(&p, count) // p += count AtomExc(p, count)

#else

#define Lock(p)
#define UnLock(p) 
#define LockType unsigned

#endif

#define Lock(p)
#define UnLock(p) 
#define LockType unsigned

#define Check(p, str) if(!(p)){printf(str);}
#define CHECK_MEM(p) Check(p,"OUT MEMMORY\n")

#define comm_u32 unsigned
#define COMM_API
#define MAX_BUF_Q 1024

typedef struct Poll_{
	comm_u32  fd;
	comm_u32  elfd;
	struct sockaddr_in server_addr;
	LockType global_lock_;
}Poll;

typedef struct data_node_{
	void* udata;
	union val{
		int session;
		unsigned char data_type;
	};
}node;

typedef struct logi_Set_{
	const char* call_func_name_;
	const char* call_file_path_;
	const char* worker_name_;
	const char* worker_path_;
	const char* global_value_;
	const char* log_path_;
	int thread_count_;
	int port_;

}lg_set;


COMM_API const char* (init_poll_env)(const char* config_path_);
COMM_API int (init_worker_env)();
COMM_API void (release_worker_env)();
#endif
