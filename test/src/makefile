#test

#PARAM
CSUTIL_DIR 	= ../..

include $(CSUTIL_DIR)/makefile.header

BASE_DIR 	= $(CSUTIL_DIR)/base
STRATEGY_DIR= $(CSUTIL_DIR)/strategy
STORE_DIR 	= $(CSUTIL_DIR)/store
HTTP_DIR 	= $(CSUTIL_DIR)/http
SOCK_DIR 	= $(CSUTIL_DIR)/sock
TEST_BASE_DIR 	= $(CSUTIL_DIR)/test_base
OPENSSL_DIR 	= /usr/local/openssl
CURL_DIR 		= /usr/local/curl
CC 			= g++
CFLAGS 		= -g -c -Wall -fPIC -D_TOOLS_MAIN_TEST_  -I$(CSUTIL_DIR) -I$(OPENSSL_DIR)/include -I$(CURL_DIR)/include -pthread -D_XOPEN_SOURCE
LIB 		+= $(CURL_DIR)/lib/libcurl.a $(OPENSSL_DIR)/lib/libssl.a $(OPENSSL_DIR)/lib/libcrypto.a -lidn -lz -ldl
OBJS 		= $(BASE_DIR)/log.o $(BASE_DIR)/statistic_data.o $(BASE_DIR)/coding.o\
			  $(BASE_DIR)/algo.o $(BASE_DIR)/int.o $(BASE_DIR)/util.o\
			  $(BASE_DIR)/file_util.o $(BASE_DIR)/hash.o $(BASE_DIR)/time.o\
			  $(BASE_DIR)/simple_reg.o $(BASE_DIR)/reg.o $(BASE_DIR)/random.o\
			  $(BASE_DIR)/cipher.o $(BASE_DIR)/rsa_cipher.o $(BASE_DIR)/coroutine.o\
			  $(BASE_DIR)/ip.o $(BASE_DIR)/consistent_hash.o $(BASE_DIR)/bloom_filter.o\
			  $(BASE_DIR)/trie.o $(BASE_DIR)/bit_arr.o $(BASE_DIR)/search.o\
			  $(BASE_DIR)/sort.o $(BASE_DIR)/skip_list.o $(BASE_DIR)/aes_cipher.o\
			  $(BASE_DIR)/distance.o $(BASE_DIR)/md5.o $(BASE_DIR)/message_digest.o\
			  $(BASE_DIR)/mutable_buffer.o $(BASE_DIR)/event_loop.o\
			  $(BASE_DIR)/event_poll.o\
			  $(SOCK_DIR)/tcp_client.o\
			  $(BASE_DIR)/curl_http.o\
			  $(HTTP_DIR)/http_proto.o $(HTTP_DIR)/http_client.o\
			  $(TEST_BASE_DIR)/src/test_base.o $(TEST_BASE_DIR)/src/test_controller.o\
			  $(STORE_DIR)/cache/lru_cache/src/lru_cache.o\
			  $(STORE_DIR)/db/hash_db/src/hash_db.o\
			  $(STORE_DIR)/db/bit_cask/src/bit_cask_db.o\
			  unit_test_http_client.o
#			  unit_test_statistic_data.o\
#			  unit_test_algo.o\
#			  unit_test_lru_cache.o\
#			  unit_test_hash_db.o\
#			  unit_test_int.o\
#			  unit_test_util.o\
#			  unit_test_time.o\
#			  unit_test_file_util.o\
#			  unit_test_reg.o\
#			  unit_test_rsa_cipher.o\
#			  unit_test_simple_reg.o\
#			  unit_test_coroutine.o\
#			  unit_test_random.o\
#			  unit_test_consistent_hash.o\
#			  unit_test_bloom_filter.o\
#			  unit_test_trie.o\
#			  unit_test_bit_arr.o\
#			  unit_test_sort.o\
#			  unit_test_coding.o\
#			  unit_test_skip_list.o\
#			  unit_test_aes_cipher.o\
#			  unit_test_smart_ptr.o\
#			  unit_test_distance.o\
#			  unit_test_curl_http.o\
#			  unit_test_mutable_buffer.o\
#			  unit_test_topn_heap.o\
#			  unit_test_http_client.o

ifeq ($(PLATFORM), Linux)
OBJS            += $(BASE_DIR)/event_epoll.o
endif

#.PHONY
.PHONY : all test clean

all : test clean

test : $(OBJS)
	$(CC) -v -o $@ $^ $(LIB) -lpthread

clean : 
	rm -fr $(OBJS)

#OBJS
$(OBJS) : %.o : %.cc
	$(CC) $(CFLAGS) -o $@ $<
