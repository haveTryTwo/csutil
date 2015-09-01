#test

#PARAM
CCUTIL_DIR 	= ../..
BASE_DIR 	= $(CCUTIL_DIR)/base
CS_DIR 		= $(CCUTIL_DIR)/cs
CC 			= g++
CFLAGS 		= -g -c -Wall -fPIC  -I$(CCUTIL_DIR) -pthread
OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/time.o\
			  $(BASE_DIR)/ip.o $(BASE_DIR)/random.o\
			  $(BASE_DIR)/coding.o $(BASE_DIR)/msg.o\
			  $(BASE_DIR)/daemon.o $(BASE_DIR)/config.o\
			  $(BASE_DIR)/event_loop.o $(BASE_DIR)/util.o\
			  $(BASE_DIR)/load_ctrl.o $(BASE_DIR)/event_poll.o\
			  $(CS_DIR)/server.o $(CS_DIR)/client.o\
			  server_demo.o

#.PHONY
.PHONY : all server clean

all : server clean

server : $(OBJS)
	$(CC) -o $@ $^ -lpthread

clean : 
	rm -fr $(OBJS)

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
