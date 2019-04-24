#test

#PARAM
CCUTIL_DIR 	= ..

include $(CCUTIL_DIR)/makefile.header

BASE_DIR 	= $(CCUTIL_DIR)/base
OPENSSL_DIR = /usr/local/openssl
CURL_DIR 	= /usr/local/curl
CC 			= g++
CFLAGS 		= -g -c -Wall -fPIC -D_CPU_MAIN_TEST_ -D_ENABLE_SYSLOG_ -I$(CCUTIL_DIR) -I$(OPENSSL_DIR)/include -I$(CURL_DIR)/include -pthread -D_XOPEN_SOURCE
OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/random.o $(BASE_DIR)/ip.o\
		  	$(BASE_DIR)/time.o $(BASE_DIR)/load_ctrl.o $(BASE_DIR)/event_poll.o\
		  	$(BASE_DIR)/coding.o $(BASE_DIR)/msg.o \
		  	$(BASE_DIR)/event_loop.o $(BASE_DIR)/config.o\
			$(BASE_DIR)/statistic.o $(BASE_DIR)/util.o\
			$(BASE_DIR)/curl_http.o $(BASE_DIR)/reg.o\
			$(BASE_DIR)/file_util.o $(BASE_DIR)/hash.o\
			$(BASE_DIR)/search.o $(BASE_DIR)/mutex.o\
			$(BASE_DIR)/int.o $(BASE_DIR)/statistic_data.o\
			$(BASE_DIR)/algo.o $(BASE_DIR)/cpu.o
LIB 		+= $(CURL_DIR)/lib/libcurl.a $(OPENSSL_DIR)/lib/libssl.a $(OPENSSL_DIR)/lib/libcrypto.a -lz -ldl

ifeq ($(PLATFORM), Linux)
OBJS 		+= $(BASE_DIR)/event_epoll.o
endif

#.PHONY
.PHONY : all build clean

all : build clean

build : $(OBJS)
	$(CC) -o $@ $^ $(LIB) -lpthread

clean : 
	rm -fr $(OBJS)

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
