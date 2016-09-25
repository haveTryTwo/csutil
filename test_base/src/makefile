#test

#PARAM
CCUTIL_DIR 	= ../..

include $(CCUTIL_DIR)/makefile.header

BASE_DIR 	= $(CCUTIL_DIR)/base
STRATEGY_DIR= $(CCUTIL_DIR)/strategy
CC 			= g++
CFLAGS 		= -g -c -Wall -fPIC -D_TOOLS_MAIN_TEST_  -I$(CCUTIL_DIR) -I$(STRATEGY_DIR) -pthread
OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/time.o\
			  $(BASE_DIR)/ip.o $(BASE_DIR)/random.o\
			  $(BASE_DIR)/coding.o $(BASE_DIR)/msg.o\
			  $(BASE_DIR)/daemon.o $(BASE_DIR)/config.o\
			  $(BASE_DIR)/event_loop.o $(BASE_DIR)/util.o $(BASE_DIR)/hash.o\
			  $(BASE_DIR)/load_ctrl.o $(BASE_DIR)/event_poll.o\
			  $(BASE_DIR)/statistic.o $(BASE_DIR)/file_util.o\
			  test_base.o test_controller.o
ifeq ($(PLATFORM), Linux)
OBJS 		+= $(BASE_DIR)/event_epoll.o
endif

#.PHONY
.PHONY : all test clean

all : test clean

test : $(OBJS)
	$(CC) -o $@ $^ $(LIB) -lpthread

clean : 
	rm -fr $(OBJS)

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
