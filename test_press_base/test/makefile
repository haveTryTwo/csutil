#test

#PARAM
CCUTIL_DIR 	= ../..

include $(CCUTIL_DIR)/makefile.header

BASE_DIR 	= $(CCUTIL_DIR)/base
SOCK_DIR 	= $(CCUTIL_DIR)/sock
HTTP_DIR 	= $(CCUTIL_DIR)/http
STRATEGY_DIR= $(CCUTIL_DIR)/strategy
TEST_BASE_DIR 	= $(CCUTIL_DIR)/test_press_base
CC 			= g++
CFLAGS 		= -g -c -Wall -fPIC -D_TOOLS_MAIN_TEST_  -I$(CCUTIL_DIR) -pthread
OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/util.o $(BASE_DIR)/ip.o $(BASE_DIR)/event_loop.o\
			  $(BASE_DIR)/event_poll.o $(BASE_DIR)/time.o $(BASE_DIR)/random.o\
			  $(BASE_DIR)/reg.o $(BASE_DIR)/coding.o $(BASE_DIR)/int.o\
			  $(HTTP_DIR)/http_client.o $(HTTP_DIR)/http_proto.o $(SOCK_DIR)/tcp_client.o\
			  $(TEST_BASE_DIR)/src/test_press_base.o $(TEST_BASE_DIR)/src/test_press_controller.o\
			  $(TEST_BASE_DIR)/src/test_busi_client.o $(TEST_BASE_DIR)/src/test_rpc_client.o\
			  $(TEST_BASE_DIR)/src/test_http_client.o\
			  my_press_test.o press_http.o press_rpc.o

ifeq ($(PLATFORM), Linux)
OBJS 		+= $(BASE_DIR)/event_epoll.o
endif


#.PHONY
.PHONY : all press_test clean

all : press_test clean

press_test : $(OBJS)
	$(CC) -o $@ $^ $(LIB) -lpthread

clean : 
	rm -fr $(OBJS)

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
