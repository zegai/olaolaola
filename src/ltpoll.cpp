#include "ltpoll.h"
#include "comm.h"
#define PRINT(p) printf("%s\n", p)


int print_tip()
{
	PRINT("please input config file path and name");
	return 0;
}


int main(int argc, char** argv)
{
	if( !argc )
		return print_tip();
	/* assert argv[0] is a lua file path*/
	init_poll_env(argv[0]);
	int handle = init_worker_env();
	while(handle){
		pthread_create();
		handle = init_worker_env();
	}
	main_socket_loop_();
	lua_tostring
//	init_worker_env
//	create_thread()
//	main_socket_loop_()

}