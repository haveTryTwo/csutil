#!/bin/sh

cd "$(dirname "$0")" || exit 1

## 先停掉可能残留的同名实例，避免端口占用
./stop.sh 2>/dev/null
sleep 1

## 提升单进程文件描述符上限：
## web 服务 real_worker_thread_num=16(每个 1 对 pipe) + 监听/连接/leveldb 句柄，
## macOS 默认 ulimit -n=256 可能导致 pipe() 失败(kPipeFailed)。抬到 8192（不超过硬上限）。
FD_TARGET=8192
FD_HARD=$(ulimit -Hn 2>/dev/null)
if [ "$FD_HARD" != "unlimited" ] && [ -n "$FD_HARD" ] && [ "$FD_HARD" -lt "$FD_TARGET" ] 2>/dev/null; then
  FD_TARGET=$FD_HARD
fi
ulimit -n "$FD_TARGET" 2>/dev/null
echo "ulimit -n set to: $(ulimit -n)"

mkdir -p ./data ./log

## 单进程 Web Agent 服务：静态问答页 + /api/agent/* REST/SSE。
## conf/ web/ skills/ data/ 均相对本目录（cwd=agent/）。
## 注：list_books/get_book 工具需 book Control 后端（sock/demo_book）在线方可命中，问答本身不依赖它。
./bin/agent_web_server > ./log/web.log 2>&1 &
sleep 1

echo "agent_web_server started: web(8080)"
echo "log: ./log/web.log"
echo "open: http://127.0.0.1:8080"
