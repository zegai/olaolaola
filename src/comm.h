#ifndef _COMM_H_
#define _COMM_H_

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



#endif

#define CHECK(p, str) {if(!p) printf("check err: %s \n", str);}
#define CHECK_MEM(p) CHECK(p, "OUT MEMMORY")

#define comm_u32 unsigned
#define COMM_API
#define MAX_BUF_Q 1024



typedef struct data_node_{
	void* udata;
	int session;
	unsigned char data_type;
}node;

typedef struct logi_Set_{
	const char* call_func_name_;
	const char* call_file_path_;
	const char* log_path_;
	int thread_count_;
}lg_set;

COMM_API const char* (init_poll)(const char* config_path_);
COMM_API const char* (init_call)(lua_State* state);

#endif