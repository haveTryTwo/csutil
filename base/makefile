#test

#PARAM
CCUTIL_DIR 	= ..
BASE_DIR 	= $(CCUTIL_DIR)/base
CC 		= g++
CFLAGS 	= -g -c -Wall -fPIC -D_TIME_MAIN_TEST_ -I$(CCUTIL_DIR)
OBJS 	= $(BASE_DIR)/log.o $(BASE_DIR)/random.o $(BASE_DIR)/ip.o\
		  $(BASE_DIR)/time.o $(BASE_DIR)/load_ctrl.o $(BASE_DIR)/event_poll.o\
		  $(BASE_DIR)/coding.o $(BASE_DIR)/msg.o \
		  $(BASE_DIR)/event_loop.o

#.PHONY
.PHONY : all build clean

all : build clean

build : $(OBJS)
	$(CC) -o $@ $^

clean : 
	rm -fr $(OBJS)

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
