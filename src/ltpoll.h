#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <comm.h>

#define CALL_ACCEPT	0x01
#define CALL_RECV	0x02
#define CALL_SEND	0x04




template<
	typename _T,
	typename _Container
>
class lt_container{
	lt_container(){
		InitSpinLock(&lock_, 50);
	};
private:
	_Container container_;
	LockType lock_;
}


template<
	typename _T, 
	typename _Container = std::queue<_T>
>
class lt_poll : public lt_poll_base {
public:
	typedef _Container::value_type value_type;
	typedef lt_container<_T, _Container> Container_;
public:
	lt_poll();
	~lt_poll();
private:
	Container_ contain;
}


class lt_poll_factory{
public:
	virtual ~lt_poll_factory();
public:
	static void pollfunc(void* udata);
	static ltpoll* GetInstance(){ return ins;};
public:
	
public:
	
public:
private:
	explicit ltpoll();
	
	static ltpoll* ins;
};




template<typename _T>
class ltpolldecode{
public:
	typedef _T ValueType;
	typedef void ValueType::*BeCb(UnionData<ValueType>*);
	typedef unsigned char CallMask;
public:
	void ltAccept(ltpoll<ValueType>*);
	void ltRecv(ltpoll<ValueType>*);
	void ltSend(ltpoll<ValueType>*);
	void SetCb(BeCb func,const UnionData<ValueType>& pud){
		retcb_ = func;
		new ud_ UnionData<ValueType>(&pud); 
	};
	void CallCb(){assert(retcb_ && ud_);retcb_(ud_)};

	void SetMask(CallMask pmask){mask = pmask;};
	void SetCallAccept(){mask |= CALL_ACCEPT;};
	void SetCallRecv(){mask |= CALL_RECV;};
	void SetCallSend(){mask |= CALL_SEND;};
public:
	explicit ltpolldecode();
	virtual ~ltpolldecode();
protected:
private:
	lua_State * WorkStatement;
	BeCb retcb_;
	UnionData<ValueType>* ud_;
	CallMask mask;
}

