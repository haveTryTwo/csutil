#test

#PARAM
CCUTIL_DIR 	= ..

include $(CCUTIL_DIR)/makefile.header

BASE_DIR 	= $(CCUTIL_DIR)/base
SOCK_DIR  	= $(CCUTIL_DIR)/sock
HTTP_DIR  	= $(CCUTIL_DIR)/http
CC 			= g++
CFLAGS 		= -g -c -Wall -fPIC -D_HTTP_CLIENT_MAIN_TEST_ -I$(CCUTIL_DIR) -pthread
OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/random.o $(BASE_DIR)/ip.o\
		  	$(BASE_DIR)/time.o $(BASE_DIR)/load_ctrl.o $(BASE_DIR)/event_poll.o\
		  	$(BASE_DIR)/coding.o $(BASE_DIR)/msg.o \
		  	$(BASE_DIR)/event_loop.o $(BASE_DIR)/config.o\
			$(BASE_DIR)/statistic.o $(BASE_DIR)/util.o\
			$(BASE_DIR)/reg.o $(BASE_DIR)/int.o\
		  	$(SOCK_DIR)/tcp_client.o\
			$(HTTP_DIR)/http_proto.o\
			$(HTTP_DIR)/http_client.o
ifeq ($(PLATFORM), Linux)
OBJS 		+= $(BASE_DIR)/event_epoll.o
endif

#.PHONY
.PHONY : all http_client clean

all : http_client clean

http_client : $(OBJS)
	$(CC) -o $@ $^ $(LIB) -lpthread

clean : 
	rm -fr $(OBJS)

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
