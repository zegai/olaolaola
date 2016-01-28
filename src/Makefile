COMM_HEAD = comm.h
COMM_OBJ = test.o comm.o queue.o loop.o buffer.o sock.o
COMM_INC = ../lua/src
COMM_LIB = ../lib/liblua.a
OUT_NAME = test

main : $(COMM_OBJ)
	cc -lm -ldl -lpthread -o $(OUT_NAME) $(COMM_OBJ) -I$(COMM_INC) $(COMM_LIB)
test.o :test.c comm.h
	cc -c test.c
comm.o :comm.c queue.h poll.h comm.h
	cc -c comm.c -I$(COMM_INC)
queue.o : queue.c queue.h $(COMM_HEAD)
	cc -c queue.c
buffer.o : buffer.c buffer.h queue.h $(COMM_HEAD)
	cc -c buffer.c queue.h
loop.o : loop.c loop.h buffer.h $(COMM_HEAD)
	cc -c loop.c -std=c99
sock.o : sock.c sock.h $(COMM_HEAD)
	cc -c sock.c
clean:
	rm -f $(COMM_OBJ) *.gch $(OUT_NAME)
