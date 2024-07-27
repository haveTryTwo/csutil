#test

#PARAM
CSUTIL_DIR 	= ..

include $(CSUTIL_DIR)/makefile.header

BASE_DIR 	= $(CSUTIL_DIR)/base
SOCK_DIR  	= $(CSUTIL_DIR)/sock
CC 			= g++
CFLAGS	= -g -c -Wall -std=c++11 -fPIC -D_TCP_CLIENT_MAIN_TEST_ -I$(CSUTIL_DIR) -I. -pthread
OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/random.o $(BASE_DIR)/ip.o\
					$(BASE_DIR)/time.o $(BASE_DIR)/load_ctrl.o $(BASE_DIR)/event_poll.o\
					$(BASE_DIR)/coding.o $(BASE_DIR)/msg.o \
					$(BASE_DIR)/event_loop.o $(BASE_DIR)/config.o\
					$(BASE_DIR)/statistic.o $(BASE_DIR)/util.o\
					$(BASE_DIR)/daemon.o\
					$(SOCK_DIR)/tcp_proto.o
CLIENT_OBJS = $(SOCK_DIR)/tcp_client.o
SERVER_OBJS = $(SOCK_DIR)/tcp_server.o

ifeq ($(PLATFORM), Linux)
OBJS 		+= $(BASE_DIR)/event_epoll.o
endif

#.PHONY
.PHONY : all tcp_client tcp_server clean

all : tcp_client tcp_server clean

tcp_client : $(OBJS) $(CLIENT_OBJS)
	$(CC) -o $@ $^ $(LIB) -lpthread

tcp_server : $(OBJS) $(SERVER_OBJS)
	$(CC) -o $@ $^ $(LIB) -lpthread

clean :
	rm -fr $(OBJS) $(CLIENT_OBJS) $(SERVER_OBJS)

clean_all :
	rm -fr $(OBJS) $(CLIENT_OBJS) $(SERVER_OBJS) tcp_client tcp_server

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(CLIENT_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(SERVER_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
