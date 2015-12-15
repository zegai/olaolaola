#include "comm.h"
#include "queue.h"
#include "poll.h"
#define LUACFUNC

typedef struct Poll_{
	comm_u32  fd;
	comm_u32  elfd;
	struct sockaddr_in server_addr;
	LockType global_lock_;
}Poll;

typedef struct thread_{
	int handle;
	lua_State* state;
	LockType lock_;
	bool isbusy;
}worker;

static Poll* poll_ = NULL;

static lg_set* lgset = NULL;
static worker* worker_ = NULL;
const char* create_poll_err_ = "create poll err";
const char* init_lua_err_ = "init lua err";
const char* succeed_ = "succeed";

//可换成jemalloc
static void *l_alloc (void *ud, void *ptr, size_t osize, size_t nsize) {
	(void)ud; (void)osize;  /* not used */
	if (nsize == 0) {
		//非标准C free(NULL)会出问题
		if (ptr != NULL){
			free(ptr);
		}
		return NULL;
	}
	else
		return realloc(ptr, nsize);
}

static int panic (lua_State* L) {
	printf("PANIC: unprotected error in call to Lua API ");
	printf(lua_tostring(L, -1));
	return 0;  /* return to Lua to abort */
}

static const char*
init_lua_S(lua_State* state){
	assert(state == NULL);
	state = lua_newstate(l_alloc, NULL);
	if(!state){
		return init_lua_err_;
	}
	lua_atpanic(state, &panic);
	luaL_openlibs(state);
	return "create lua state succeed.";
}

static void 
init_poll_struct(lua_State* state){
	
	assert( poll_ == NULL );
	poll_ = (Poll *)malloc(sizeof(Poll));

	CHECK_MEM(poll_)
	lua_getglobal(state, "poll_handle");
	assert(lua_isnil(state, -1));
	lua_pop(state, -1);

	lua_pushlightuserdata(state, poll_);
	lua_setglobal(state, "poll_handle");

	poll_->server_addr.sin_family = AF_INET;
	poll_->server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	poll_->server_addr.sin_port = htons(7777);

	int fd = net_init();
	assert( fd );
	int flg = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flg | O_NONBLOCK);

	poll_->server_addr.elfd = fd;

	fd = socket(AF_INET, SOCK_STREAM, 0);

	set_opt(state);
}

inline static void*  
check_udata(lua_State* state, const char* name){
	lua_getglobal(state, name);
	assert( lua_isnil(state, -1) != NULL );
	void* poll_S_ = luaL_checkudata(state, 1);
	lua_pop(state, 1);
	return poll_S_;
}

static int 
set_opt(lua_State* state){
	Poll* poll_s = (Poll*)check_udata(state);
	int value = 1;
	setsockopt(s->fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
	setsockopt(s->fd, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(int));
}

static void 
init_control(lua_State* state){
	lua_getglobal(state, "init_port");
	lgset->port_ = lua_isnumber(state, -1) ? 2333 : lua_tonumber(state, -1);
	lua_pop(state, 1);

	lua_getglobal(state, "init_thread_count");
	lgset->thread_count_ = lua_isnumber(state, -1) ? 4 : lua_tonumber(state, -1);
	lua_pop(state, 1);

	lua_getglobal(state, "init_work_path");
	lgset->worker_path_ = lua_isstring(state, -1) ? "./lua/worker.lua" : lua_tostring(state, -1);
	lua_pop(state, 1);

	lua_getglobal(state, "init_work_func");
	lgset->worker_path_ = lua_isstring(state, -1) ? "work" : lua_tostring(state, -1);
	lua_pop(state, 1);

	lua_getglobal(state, "init_log_path");
	lgset->worker_path_ = lua_isstring(state, -1) ? "./log" : lua_tostring(state, -1);
	lua_pop(state, 1);

	//lua_getglobal(state, "global_value");

}

const const char* 
init_poll_env(const char* config_){
	lua_State* state = NULL;
	if(init_lua_S(state) == init_lua_err_)
		return create_poll_err_;

	assert(state);
	init_poll_struct(state);
	void* p = check_udata(state, "poll_handle");
	if(p == NULL)
		return create_poll_err_;
	assert( lgset );
	lgset = (logi_Set_*)malloc(sizeof(logi_Set_));
	CHECK_MEM(lgset)
	lgset->call_func_name_ = "main";
	lgset->call_file_path_ = "./lua/main.lua";
	lgset->thread_count_ = 4;

	int status = luaL_dofile(state, config_);
	if( status != 0 ){
		printf("do file err : %s\n", lua_tostring(state, -1));
		lua_pop(state, 1);
		return create_poll_err_;
	}

	if( lgset->thread_count_ ){
		worker_ = (worker*)malloc(sizeof(worker)*lgset->thread_count_);
		CHECK_MEM(worker_)
		if(!worker_)
			return create_poll_err_;
		memset(worker_, 0, sizeof(worker)*lgset->thread_count_);
	}

	return "succeed";
}

const lg_set*
get_lgset(){
	return lgset;
}

//return thread count 
static int
init_worker_env(){
	int t_count = lgset->thread_count_;
	lua_State* tmp_state = NULL;
	printf("create thread state:%s\n", init_lua_S(tmp_state));
	assert( lgset );
	int handle = 0;

	if( worker_[lgset->thread_count_ - 1]->state ){
		return -1;
	}

	for(; handle < lgset->thread_count_ ; handle ++){
		if( !worker_[handle].state )
			break;
	}

	worker_[handle].state = tmp_state;
	worker_[handle].handle = handle;

	lua_pushinteger(tmp_state, handle);
	lua_setglobal(tmp_state, "thread_");
	int v = luaL_loadfile( tmp_state, lgset->call_file_path_ );
	if( v != LUA_OK ){
		printf("lua check err:%s\n", lua_tostring(tmp_state, -1));
		return -1;
	}
	lua_pcall( tmp_state, 0, 0, 0 );
	return handle;
}

static void
release_worker_env(){
	int t_count = lgset->thread_count_;
	int i = 0;
	for(;i < t_count; i++){
		while( *(worker_ + i)->isbusy ){
			sleep(10);
		}
		//save_data()
		lua_close(*(worker_ + i)->state);
	}
}

//main(int argc, char** argv)
//init_poll_env
//for(0->thread_count)
//	init_worker_env
//	create_thread()
//main_socket_loop_()
//return 0;

//block
static void*
main_socket_loop_(void* udata){
	CHECK(poll_, "POLL ENV NEED INIT")
	Poll* loop_p = poll_;
	
}

//get a node do a step
static int
worker_loop_(int handle, node* nnode){
	assert( nnode && worker_);
	//thread struct
	worker* work_handle = worker_[handle];
	
}
