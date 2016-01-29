#include "comm.h"

int main(int argc, char **argv){
	init_poll_env("./service/start.lua");
	int i = -1;
	start_loop();
	return 0;
}