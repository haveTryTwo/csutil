# book_mgr demo: RPC CRUD system (control + leveldb dao + cache dao)

#PARAM
CSUTIL_DIR 	= ../..

include $(CSUTIL_DIR)/makefile.header

BASE_DIR 	= $(CSUTIL_DIR)/base
SOCK_DIR  	= $(CSUTIL_DIR)/sock
STORE_DIR 	= $(CSUTIL_DIR)/store
PROTOBUF_DIR = $(CSUTIL_DIR)/third_party_install/protobuf-2.6.1
LEVELDB_DIR = $(CSUTIL_DIR)/../leveldb-1.15.0
OPENSSL_DIR = $(CSUTIL_DIR)/third_party_install/openssl-3.3.2
SNAPPY_DIR  = $(CSUTIL_DIR)/third_party_install/snappy-1.1.4
ZSTD_DIR 	= $(CSUTIL_DIR)/third_party_install/zstd-1.5.6
CURL_DIR 	= $(CSUTIL_DIR)/third_party_install/curl-7.63.0
ZLIB_DIR 	= $(CSUTIL_DIR)/third_party_install/zlib-1.2.13
PB_SRC_DIR  = $(SOCK_DIR)/demo_book/proto
RAPIDJSON_DIR = $(CSUTIL_DIR)/third_party/rapidjson-master-20190827/include

CC 			= g++
CFLAGS	= -g -c -Wall -std=c++11 -fPIC -I$(CSUTIL_DIR) -I$(PROTOBUF_DIR)/include\
			  -I$(LEVELDB_DIR)/include -I$(RAPIDJSON_DIR) -I$(CURL_DIR)/include -I. -pthread

# libcurl 静态链接所需依赖（由 curl-config --static-libs 得到）
ifeq ($(PLATFORM), Darwin)
CURL_SYS_LDFLAGS = -framework CoreFoundation -framework SystemConfiguration -framework Security -lldap
else
CURL_SYS_LDFLAGS = -lldap
endif
CURL_LIBS = $(CURL_DIR)/lib/libcurl.a -L$(ZLIB_DIR)/lib -L$(OPENSSL_DIR)/lib -lssl -lcrypto -lz $(CURL_SYS_LDFLAGS)

OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/random.o $(BASE_DIR)/ip.o\
					$(BASE_DIR)/time.o $(BASE_DIR)/load_ctrl.o $(BASE_DIR)/event_poll.o\
					$(BASE_DIR)/coding.o $(BASE_DIR)/msg.o \
					$(BASE_DIR)/event_loop.o $(BASE_DIR)/config.o\
					$(BASE_DIR)/statistic.o $(BASE_DIR)/util.o\
					$(BASE_DIR)/daemon.o\
					$(SOCK_DIR)/rpc_proto.o\
					$(SOCK_DIR)/tcp_client.o $(SOCK_DIR)/rpc_client.o $(SOCK_DIR)/rpc_channel.o\
					$(SOCK_DIR)/rpc_conn_pool.o\
					$(SOCK_DIR)/load_balancer.o $(SOCK_DIR)/file_service_provider.o\
					$(SOCK_DIR)/service_manager.o\
					$(SOCK_DIR)/rpc_server.o

ifeq ($(PLATFORM), Linux)
OBJS 		+= $(BASE_DIR)/event_epoll.o
endif

PB_OBJS = $(PB_SRC_DIR)/book.pb.o
WRAPPER_OBJS = $(SOCK_DIR)/demo_book/leveldb_wrapper.o
CACHE_OBJS = $(STORE_DIR)/cache/lru_cache/src/lru_cache.o

RPC_CLIENT_OBJS = $(SOCK_DIR)/demo_book/rpc_client_book_demo.o
SERVER_CONTROL_OBJS = $(SOCK_DIR)/demo_book/rpc_server_book_control_demo.o
SERVER_LEVELDB_OBJS = $(SOCK_DIR)/demo_book/rpc_server_book_leveldb_dao_demo.o
SERVER_CACHE_OBJS = $(SOCK_DIR)/demo_book/rpc_server_book_cache_dao_demo.o
SERVER_WEB_OBJS = $(SOCK_DIR)/demo_book/rpc_server_book_web_demo.o
AGENT_OBJS = $(SOCK_DIR)/demo_book/agent/agent_config.o $(SOCK_DIR)/demo_book/agent/skill_registry.o\
			$(SOCK_DIR)/demo_book/agent/llm_client.o $(SOCK_DIR)/demo_book/agent/agent_service.o\
			$(SOCK_DIR)/demo_book/agent/session_store.o $(SOCK_DIR)/demo_book/agent/knowledge_retriever.o\
			$(SOCK_DIR)/demo_book/agent/memory_store.o $(SOCK_DIR)/demo_book/agent/tool_executor.o

PB_LIB = $(PROTOBUF_DIR)/lib/libprotobuf.a
LEVELDB_LIB = $(LEVELDB_DIR)/libleveldb.a $(SNAPPY_DIR)/lib/libsnappy.a\
			  $(ZSTD_DIR)/lib/libzstd.a $(OPENSSL_DIR)/lib/libcrypto.a

ifeq ($(PLATFORM), Darwin)
LEVELDB_LDFLAGS = -framework CoreFoundation -framework SystemConfiguration
endif

#.PHONY
.PHONY : all proto rpc_client rpc_server_book_control rpc_server_book_leveldb_dao rpc_server_book_cache_dao rpc_server_book_web clean clean_all

all : proto rpc_client rpc_server_book_control rpc_server_book_leveldb_dao rpc_server_book_cache_dao rpc_server_book_web clean

proto :
	$(PROTOBUF_DIR)/bin/protoc --proto_path=$(PB_SRC_DIR) --cpp_out=$(PB_SRC_DIR) $(PB_SRC_DIR)/book.proto

rpc_client : $(OBJS) $(PB_OBJS) $(RPC_CLIENT_OBJS)
	$(CC) -o $@ $^ $(LIB) $(PB_LIB) -lpthread

rpc_server_book_control : $(OBJS) $(PB_OBJS) $(SERVER_CONTROL_OBJS)
	$(CC) -o $@ $^ $(LIB) $(PB_LIB) -lpthread

rpc_server_book_leveldb_dao : $(OBJS) $(PB_OBJS) $(WRAPPER_OBJS) $(SERVER_LEVELDB_OBJS)
	$(CC) -o $@ $^ $(LIB) $(PB_LIB) $(LEVELDB_LIB) $(LEVELDB_LDFLAGS) -lpthread

rpc_server_book_cache_dao : $(OBJS) $(PB_OBJS) $(CACHE_OBJS) $(SERVER_CACHE_OBJS)
	$(CC) -o $@ $^ $(LIB) $(PB_LIB) -lpthread

rpc_server_book_web : $(OBJS) $(PB_OBJS) $(AGENT_OBJS) $(WRAPPER_OBJS) $(SERVER_WEB_OBJS)
	$(CC) -o $@ $^ $(LIB) $(PB_LIB) $(LEVELDB_LIB) $(LEVELDB_LDFLAGS) $(CURL_LIBS) -lpthread

clean :
	rm -fr $(OBJS) $(PB_OBJS) $(WRAPPER_OBJS) $(CACHE_OBJS)\
		$(RPC_CLIENT_OBJS) $(SERVER_CONTROL_OBJS) $(SERVER_LEVELDB_OBJS) $(SERVER_CACHE_OBJS) $(SERVER_WEB_OBJS)\
		$(AGENT_OBJS)

clean_all : clean
	rm -fr rpc_client rpc_server_book_control rpc_server_book_leveldb_dao rpc_server_book_cache_dao rpc_server_book_web

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(PB_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(WRAPPER_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(CACHE_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(RPC_CLIENT_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(SERVER_CONTROL_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(SERVER_LEVELDB_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(SERVER_CACHE_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(SERVER_WEB_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(AGENT_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
