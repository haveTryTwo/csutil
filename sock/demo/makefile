#test

#PARAM
CSUTIL_DIR 	= ../..

include $(CSUTIL_DIR)/makefile.header

BASE_DIR 	= $(CSUTIL_DIR)/base
SOCK_DIR  	= $(CSUTIL_DIR)/sock
PROTOBUF_DIR = $(CSUTIL_DIR)/third_party_install/protobuf-2.6.1
PB_SRC_DIR   = $(SOCK_DIR)/demo_multi/proto
CC 			= g++
CFLAGS	= -g -c -Wall -std=c++11 -fPIC -D_TCP_CLIENT_MAIN_TEST_ -I$(CSUTIL_DIR) -I$(PROTOBUF_DIR)/include -I. -pthread
OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/random.o $(BASE_DIR)/ip.o\
					$(BASE_DIR)/time.o $(BASE_DIR)/load_ctrl.o $(BASE_DIR)/event_poll.o\
					$(BASE_DIR)/coding.o $(BASE_DIR)/msg.o \
					$(BASE_DIR)/event_loop.o $(BASE_DIR)/config.o\
					$(BASE_DIR)/statistic.o $(BASE_DIR)/util.o\
					$(BASE_DIR)/daemon.o\
					$(SOCK_DIR)/rpc_proto.o\
					$(SOCK_DIR)/tcp_client.o $(SOCK_DIR)/rpc_client.o $(SOCK_DIR)/rpc_server.o
PB_OBJS = $(PB_SRC_DIR)/demo_multi.pb.o
CLIENT_OBJS = $(SOCK_DIR)/demo/tcp_client_demo.o
RPC_CLIENT_OBJS = $(SOCK_DIR)/demo/rpc_client_demo.o
SERVER_OBJS = $(SOCK_DIR)/demo/rpc_server_demo.o

ifeq ($(PLATFORM), Linux)
OBJS 		+= $(BASE_DIR)/event_epoll.o
endif

PB_LIB = $(PROTOBUF_DIR)/lib/libprotobuf.a

#.PHONY
.PHONY : all tcp_client rpc_client rpc_server clean

all : tcp_client rpc_client rpc_server clean

tcp_client : $(OBJS) $(PB_OBJS) $(CLIENT_OBJS)
	$(CC) -o $@ $^ $(LIB) $(PB_LIB) -lpthread

rpc_client : $(OBJS) $(PB_OBJS) $(RPC_CLIENT_OBJS)
	$(CC) -o $@ $^ $(LIB) $(PB_LIB) -lpthread

rpc_server : $(OBJS) $(PB_OBJS) $(SERVER_OBJS)
	$(CC) -o $@ $^ $(LIB) $(PB_LIB) -lpthread

clean :
	rm -fr $(OBJS) $(PB_OBJS) $(CLIENT_OBJS) $(RPC_CLIENT_OBJS) $(SERVER_OBJS)

clean_all :
	rm -fr $(OBJS) $(PB_OBJS) $(CLIENT_OBJS) $(RPC_CLIENT_OBJS) $(SERVER_OBJS) tcp_client rpc_client rpc_server

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(PB_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(CLIENT_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(RPC_CLIENT_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(SERVER_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
