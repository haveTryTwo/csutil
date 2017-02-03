# strategy makefile

# PARAM
CSUTIL_DIR 	= ..
BASE_DIR 	= $(CSUTIL_DIR)/base
CC 			= g++
CCFLAGS 	= -g -Wall -fPIC -D_SINGLETON_MAIN_TEST_ -I$(CSUTIL_DIR) -pthread
OBJS 		= singleton.o

# PHONY
.PHONY : all build clean

all : build clean

build : $(OBJS)
	$(CC) -o $@ $^ -lpthread

clean : 
	rm -fr $(OBJS)

# OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CCFLAGS) -c -o $@ $<
