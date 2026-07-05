# agent module: standalone Web Agent server (independent of book CRUD)
# 运行目录约定：csutil/agent（conf/ web/ skills/ data/ 均相对该目录）

#PARAM
CSUTIL_DIR 	= ..

include $(CSUTIL_DIR)/makefile.header

BASE_DIR 	= $(CSUTIL_DIR)/base
SOCK_DIR  	= $(CSUTIL_DIR)/sock
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

# 复用 base / sock 框架对象（由各自 makefile 或此处 pattern 规则编译）
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

# book proto：仅供 Agent 的 book 工具（list_books/get_book）调用 Control 使用
PB_OBJS = $(PB_SRC_DIR)/book.pb.o

# Agent 核心库对象（本模块自有，零业务依赖）
AGENT_LIB_OBJS = src/agent_config.o src/skill_registry.o src/knowledge_retriever.o\
				 src/llm_client.o src/agent_service.o src/session_store.o\
				 src/memory_store.o src/tool_executor.o src/kv_store.o

# Web 服务入口（宿主：装配 Agent + 注入 book 工具回调）
SERVER_OBJS = src/agent_web_server.o

# 可执行文件输出目录
BIN_DIR = bin
TARGET  = $(BIN_DIR)/agent_web_server

PB_LIB = $(PROTOBUF_DIR)/lib/libprotobuf.a
LEVELDB_LIB = $(LEVELDB_DIR)/libleveldb.a $(SNAPPY_DIR)/lib/libsnappy.a\
			  $(ZSTD_DIR)/lib/libzstd.a $(OPENSSL_DIR)/lib/libcrypto.a

ifeq ($(PLATFORM), Darwin)
LEVELDB_LDFLAGS = -framework CoreFoundation -framework SystemConfiguration
endif

#.PHONY
.PHONY : all proto agent_web_server clean clean_all

all : proto agent_web_server clean

proto :
	$(PROTOBUF_DIR)/bin/protoc --proto_path=$(PB_SRC_DIR) --cpp_out=$(PB_SRC_DIR) $(PB_SRC_DIR)/book.proto

agent_web_server : $(TARGET)

$(TARGET) : $(OBJS) $(PB_OBJS) $(AGENT_LIB_OBJS) $(SERVER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(LIB) $(PB_LIB) $(LEVELDB_LIB) $(LEVELDB_LDFLAGS) $(CURL_LIBS) -lpthread

clean :
	rm -fr $(AGENT_LIB_OBJS) $(SERVER_OBJS)

clean_all : clean
	rm -fr $(TARGET)

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(PB_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(AGENT_LIB_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<

$(SERVER_OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
