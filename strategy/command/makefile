# strategy makefile

# PARAM
CCUTIL_DIR 	= ../..
BASE_DIR 	= $(CCUTIL_DIR)/base
CC 			= g++
CCFLAGS 	= -g -Wall -fPIC -D_INVOKER_MAIN_TEST_ -I$(CCUTIL_DIR) -pthread
OBJS 		= command.o timer_command.o timer_command_queue.o lineup_command_queue.o invoker.o
BASE_OBJS 	= $(BASE_DIR)/log.o $(BASE_DIR)/time.o

# PHONY
.PHONY : all build clean

all : build clean

build : $(OBJS) $(BASE_OBJS)
	$(CC) -o $@ $^ -lpthread

clean : 
	rm -fr $(OBJS) $(BASE_OBJS)

# OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CCFLAGS) -c -o $@ $<

$(BASE_OBJS) : %.o : %.cc
	$(CC) $(CCFLAGS) -c -o $@ $<
