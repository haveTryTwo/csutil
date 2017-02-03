#test

#PARAM
CSUTIL_DIR 	= ../../../..

include $(CSUTIL_DIR)/makefile.header

BASE_DIR 	= $(CSUTIL_DIR)/base
PROTO_DIR 	= ./proto_src
PROTOBUF_DIR= /usr/local/protobuf
CC 			= g++
CFLAGS 		= -g -c -Wall -fPIC -D_LRU_CACHE_MAIN_TEST_  -I$(CSUTIL_DIR) -I$(PROTO_DIR) -pthread\
			  -I$(PROTOBUF_DIR)/include
OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/time.o\
			  $(BASE_DIR)/ip.o $(BASE_DIR)/random.o\
			  $(BASE_DIR)/coding.o $(BASE_DIR)/msg.o\
			  $(BASE_DIR)/daemon.o $(BASE_DIR)/config.o\
			  $(BASE_DIR)/event_loop.o $(BASE_DIR)/util.o $(BASE_DIR)/hash.o\
			  $(BASE_DIR)/load_ctrl.o $(BASE_DIR)/event_poll.o\
			  $(BASE_DIR)/statistic.o $(BASE_DIR)/file_util.o\
			  lru_cache.o

ifeq ($(PLATFORM), Linux)
OBJS 		+= $(BASE_DIR)/event_epoll.o
endif
LIB 		+= $(PROTOBUF_DIR)/lib/libprotobuf.a

#.PHONY
.PHONY : all build clean

all : build clean

build : $(OBJS) $(PROTO_OBJS)
	$(CC) -o $@ $^ $(LIB) -lpthread

clean : 
	rm -fr $(OBJS) $(PROTO_OBJS)

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(PROTO_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
