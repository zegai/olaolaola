#include "comm.h"

int main(int argc, char **argv){
	init_poll_env("./service/start.lua");
	int i = -1;
	for(;;){
		i = init_worker_env();
		if( i ){
			printf("Init Worker Thread : \n");
		}
		else{
			printf("Out Worker Thread Count\n");
			break;
		}
	}
	return 0;
}