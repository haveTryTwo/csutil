# 图书管理 CRUD 系统（sock RPC 框架模板实践）

基于 `csutil/sock` RPC 框架实现的三服务图书管理系统，演示「逻辑控制层 + 多 DAO 后端」
的编排模式，可作为后续更复杂业务服务的脚手架模板。

设计文档见：[`docs/BOOK_MGR_RPC_DESIGN.md`](../../docs/BOOK_MGR_RPC_DESIGN.md)

## 架构

```
                 +---------------------------+
   client  --->  |  Control (9101)           |   逻辑控制层：解析 oneof，按
                 |  rpc_server_book_control  |   Cache-Aside 编排两个 DAO
                 +------------+--------------+
                              |
              +---------------+----------------+
              |                                |
   +----------v-----------+      +-------------v------------+
   | Cache DAO (9105)     |      | LevelDB DAO (9103)       |
   | LRU 缓存（内存）      |      | LevelDB 持久化（磁盘）    |
   +----------------------+      +--------------------------+
```

- **Control 控制层**：对外提供统一入口，解析 `BookReq.oneof` 判定 CRUD 类型，
  按 Cache-Aside 策略编排后端：读先查缓存未命中回源并回填；写/删先落库再失效缓存。
- **LevelDB DAO**：封装 `leveldb-1.15.0` 静态库，提供 KV 持久化（key=book_id，value=序列化 Book）。
- **Cache DAO**：封装 `store/cache/lru_cache`，提供内存缓存。

## 业务协议（protobuf + oneof）

`proto/book.proto` 将 CRUD 四组 req/resp 聚合到顶层 `BookReq` / `BookResp` 的 `oneof` 中，
各服务通过 `req_body_case()` 判定具体操作类型并分支处理。

## 构建

依赖：内置 protobuf 2.6.1（`third_party_install/`）、LevelDB 静态库
（`../..//git/leveldb-1.15.0/libleveldb.a`）。

```bash
cd sock/demo_book
make all          # 生成 proto 代码并编译 4 个二进制
# make clean_all  # 清理二进制与中间产物
```

产物：`rpc_server_book_control`、`rpc_server_book_leveldb_dao`、
`rpc_server_book_cache_dao`、`rpc_client`。

## 运行

手动启动（前台/后台均可，端口见各 `conf/*_server.conf`）：

```bash
./start.sh        # 依次启动 leveldb_dao(9103)、cache_dao(9105)、control(9101)
./rpc_client      # 执行一轮 CRUD 模拟
./stop.sh         # 关闭所有服务
```

一键联调（启动 -> 跑客户端 -> 关闭，适合本地快速验证）：

```bash
bash run_demo.sh
```

## 预期输出（Cache-Aside 行为）

```
[1] CREATE                       -> ret_code=0 success
[2] READ  (首次)                 -> from_cache=false（缓存未命中，回源 LevelDB 并回填）
[3] READ  (再次)                 -> from_cache=true （命中缓存）
[4] UPDATE                       -> ret_code=0 success（落库后失效缓存）
[5] READ  (更新后)               -> from_cache=false，price 为新值（缓存已失效，回源取新值）
[6] DELETE                       -> ret_code=0 success（删库后失效缓存）
[7] READ  (删除后)               -> ret_code=2 book not found
```

## 目录结构

```
demo_book/
├── proto/book.proto                       # 业务协议（oneof）
├── leveldb_wrapper.h/.cc                   # LevelDB 静态库封装
├── book_common.h                          # oneof 构造/解析、配置键、返回码
├── rpc_server_book_control_demo.cc        # 逻辑控制层
├── rpc_server_book_leveldb_dao_demo.cc    # LevelDB DAO
├── rpc_server_book_cache_dao_demo.cc      # LRU Cache DAO
├── rpc_client_book_demo.cc                # 客户端模拟器
├── conf/                                   # 各服务配置
├── makefile / start.sh / stop.sh / run_demo.sh
├── press/                                  # 压测程序（见下）
└── README.md
```

## 压测

基于通用压测框架 `test_press_base`（新增 `sock_rpc` 协议客户端，复用 sock 协议编解码），
可对 control / leveldb_dao / cache_dao 任一层进行性能压测。详见
[`docs/BOOK_MGR_RPC_DESIGN.md` 第 12 节](../../docs/BOOK_MGR_RPC_DESIGN.md)。

```bash
cd press
make                       # 构建 press_test
bash bench_single.sh       # 三层单连接基线对比
bash sweep.sh              # 并发拐点扫描
bash run_press.sh 50 10    # 一键多场景（起服务->预热->压测->停服务）
```

> 压测启动服务务必设 `BOOK_LOG_LEVEL=5` 屏蔽框架逐请求 trace（脚本已内置）。
> 本地实测：单连接纯内存 ~2 万 QPS；Control 端到端 ~1.2k QPS（瓶颈为每请求新建后端连接，
> 建议引入连接池优化）。

## 扩展指引

- **新增字段**：修改 `proto/book.proto` 的 `Book`，重新 `make` 即可。
- **新增操作**：在 `oneof` 中增加 req/resp，并在三个服务的 `switch(req_body_case())` 中补充分支。
- **替换存储**：实现新的 DAO 服务（复制 leveldb/cache dao 结构），Control 增加对应编排路由。
