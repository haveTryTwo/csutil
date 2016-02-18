#test

#PARAM
CCUTIL_DIR 	= ..

include $(CCUTIL_DIR)/makefile.header

BASE_DIR 	= $(CCUTIL_DIR)/base
CC 			= g++
CFLAGS 		= -g -c -Wall -fPIC -D_SEARCH_MAIN_TEST_ -I$(CCUTIL_DIR) -pthread
OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/random.o $(BASE_DIR)/ip.o\
		  	$(BASE_DIR)/time.o $(BASE_DIR)/load_ctrl.o $(BASE_DIR)/event_poll.o\
		  	$(BASE_DIR)/coding.o $(BASE_DIR)/msg.o \
		  	$(BASE_DIR)/event_loop.o $(BASE_DIR)/config.o\
			$(BASE_DIR)/statistic.o $(BASE_DIR)/util.o\
			$(BASE_DIR)/curl_http.o $(BASE_DIR)/reg.o\
			$(BASE_DIR)/file_util.o $(BASE_DIR)/hash.o\
			$(BASE_DIR)/search.o\
			$(BASE_DIR)/int.o
LIB 		+= -lcurl

ifeq ($(PLATFORM), Linux)
OBJS 		+= $(BASE_DIR)/event_epoll.o
endif

#.PHONY
.PHONY : all build clean

all : build clean

build : $(OBJS)
	$(CC) -o $@ $^ $(LIB)

clean : 
	rm -fr $(OBJS)

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
