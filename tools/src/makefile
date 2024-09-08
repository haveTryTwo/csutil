#test

#PARAM
CSUTIL_DIR 	= ../..
PB_SRC      = ./pb_src
PB_PATH     = ../proto

include $(CSUTIL_DIR)/makefile.header

BASE_DIR 	= $(CSUTIL_DIR)/base
DATA_PROCESS_DIR = $(CSUTIL_DIR)/data_process
PROTO_DIR 	= $(CSUTIL_DIR)/proto
PROTOBUF_DIR  = /usr/local/protobuf
RAPID_JSON_DIR  = $(CSUTIL_DIR)/third_party/rapidjson-master-20190827
CC 			= g++
CFLAGS 		= -g -c -Wall -std=c++11 -fPIC -D_TOOLS_MAIN_TEST_  -I$(CSUTIL_DIR) -pthread -I$(RAPID_JSON_DIR)/include  -I$(PROTOBUF_DIR)/include
LIB += $(PROTOBUF_DIR)/lib/libprotobuf.a
PB_OBJS   = $(PB_SRC)/base.pb.o
OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/time.o\
			  $(BASE_DIR)/int.o\
			  $(BASE_DIR)/ip.o $(BASE_DIR)/random.o\
			  $(BASE_DIR)/coding.o $(BASE_DIR)/msg.o\
			  $(BASE_DIR)/daemon.o $(BASE_DIR)/config.o\
			  $(BASE_DIR)/event_loop.o $(BASE_DIR)/util.o $(BASE_DIR)/hash.o\
			  $(BASE_DIR)/load_ctrl.o $(BASE_DIR)/event_poll.o\
			  $(BASE_DIR)/statistic.o $(BASE_DIR)/file_util.o\
			  $(PROTO_DIR)/pb_util.o\
			  $(DATA_PROCESS_DIR)/src/data_process.o\
			  log_check.o file_content_replace.o tools.o create_cc_file.o\
			  create_java_file.o init_json_value.o
ifeq ($(PLATFORM), Linux)
OBJS 		+= $(BASE_DIR)/event_epoll.o
endif

#.PHONY
.PHONY : all tools clean

all : create_pb tools clean

create_pb:
	  mkdir -p $(PB_SRC)
	  export LD_LIBRARY_PATH=${PROTOBUF_DIR}/lib; $(PROTOBUF_DIR)/bin/protoc --cpp_out=$(PB_SRC) --proto_path=$(PB_PATH) $(PB_PATH)/*.proto


tools : $(OBJS) $(PB_OBJS)
	$(CC) -o $@ $^ $(LIB) -lpthread

clean : 
	rm -fr $(OBJS)

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(PB_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
