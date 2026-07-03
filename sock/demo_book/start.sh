#!/bin/sh

cd "$(dirname "$0")" || exit 1

## 先停掉可能残留的同名实例，避免端口占用 / 资源累积导致 RpcServer::Init 失败
./stop.sh 2>/dev/null
sleep 1

## 提升单进程文件描述符上限：
## 每个服务 real_worker_thread_num=100(每个 1 对 pipe) + conn_worker_thread_num=20(每个 2 对 pipe)
## 约需 100*2 + 20*4 = 280 个 fd，再加监听/连接/leveldb 句柄，macOS 默认 ulimit -n=256 会导致 pipe() 失败(kPipeFailed)。
## 这里把软上限抬到 8192（不超过硬上限），子进程(三个 server)会继承该值。
FD_TARGET=8192
FD_HARD=$(ulimit -Hn 2>/dev/null)
if [ "$FD_HARD" != "unlimited" ] && [ -n "$FD_HARD" ] && [ "$FD_HARD" -lt "$FD_TARGET" ] 2>/dev/null; then
  FD_TARGET=$FD_HARD
fi
ulimit -n "$FD_TARGET" 2>/dev/null
echo "ulimit -n set to: $(ulimit -n)"

mkdir -p ./data ./log

## 先启动两个 DAO 后端，再启动控制层；日志落到 ./log，便于排查启动失败原因
## 如需压测，可在每个命令前加 BOOK_LOG_LEVEL=5 屏蔽框架逐请求 trace
./rpc_server_book_leveldb_dao > ./log/leveldb_dao.log 2>&1 &
sleep 1

./rpc_server_book_cache_dao > ./log/cache_dao.log 2>&1 &
sleep 1

./rpc_server_book_control > ./log/control.log 2>&1 &
sleep 1

## HTTP 网关：浏览器 CRUD 页面 + REST API，转发到 control(9101)
./rpc_server_book_web > ./log/web.log 2>&1 &
sleep 1

echo "book_mgr servers started: leveldb_dao(9103), cache_dao(9105), control(9101), web(8080)"
echo "logs: ./log/{leveldb_dao,cache_dao,control,web}.log"
echo "open: http://127.0.0.1:8080"

## 执行 rpc client
## ./rpc_client
