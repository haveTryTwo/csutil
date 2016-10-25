#test

#PARAM
CCUTIL_DIR 	= ../..

include $(CCUTIL_DIR)/makefile.header

BASE_DIR 	= $(CCUTIL_DIR)/base
STRATEGY_DIR= $(CCUTIL_DIR)/strategy
TEST_BASE_DIR 	= $(CCUTIL_DIR)/test_base
CC 			= g++
CFLAGS 		= -g -c -Wall -fPIC -D_TOOLS_MAIN_TEST_  -I$(CCUTIL_DIR) -pthread
OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/statistic_data.o\
			  $(BASE_DIR)/algo.o $(BASE_DIR)/int.o\
			  $(TEST_BASE_DIR)/src/test_base.o $(TEST_BASE_DIR)/src/test_controller.o\
			  unit_test_statistic_data.o\
			  unit_test_algo.o\
			  unit_test_int.o

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
