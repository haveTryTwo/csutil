#!/bin/bash
# 一键运行：启动三服务 -> 执行客户端 CRUD -> 关闭服务
# 仅用于本地联调演示

cd "$(dirname "$0")" || exit 1

./stop.sh 2>/dev/null
rm -rf ./data
mkdir -p ./data ./log

./rpc_server_book_leveldb_dao > log/leveldb_dao.log 2>&1 &
LV=$!
./rpc_server_book_cache_dao > log/cache_dao.log 2>&1 &
CA=$!
sleep 2
./rpc_server_book_control > log/control.log 2>&1 &
CT=$!
sleep 2

echo "=== pids: leveldb=$LV cache=$CA control=$CT ==="
echo "############ CLIENT OUTPUT ############"
./rpc_client
RET=$?
echo "############ END CLIENT (ret=$RET) ############"

kill -9 "$LV" "$CA" "$CT" 2>/dev/null
echo "servers stopped"
exit $RET
