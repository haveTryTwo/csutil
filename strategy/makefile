# strategy makefile

# PARAM
CCUTIL_DIR 	= ..
BASE_DIR 	= $(CCUTIL_DIR)/base
CC 			= g++
CCFLAGS 	= -g -Wall -fPIC -D_SINGLETON_MAIN_TEST_ -I$(CCUTIL_DIR) -pthread
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
