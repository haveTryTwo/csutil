# csutil RPC 框架架构文档

## 1. 概述

`sock/` 目录实现了一个轻量级的自研 RPC 框架，基于 **TCP + 自定义二进制协议 + Protobuf** 构建。框架采用 **三层线程模型**（Main Thread → ConnWorker → RealWorker），通过 **EventLoop + pipe** 实现线程间高效通信，支持非阻塞 I/O 和可插拔的协议编解码。

---

## 2. 目录结构

```
sock/
├── base_server.h              # Server 抽象基类
├── base_client.h              # Client 抽象基类
├── rpc_proto.h / rpc_proto.cc # 协议层：编解码、粘包拆包
├── rpc_server.h / rpc_server.cc  # 服务端核心：RpcServer + ConnWorker + RealWorker
├── rpc_client.h / rpc_client.cc  # 客户端核心：RpcClient
├── tcp_client.h / tcp_client.cc  # TCP 客户端：底层 socket 收发
├── demo/                      # 单服务 demo
│   ├── conf/server.conf
│   ├── rpc_server_demo.cc     # Echo 风格 Server
│   ├── rpc_client_demo.cc     # 字符串版 Client
│   └── tcp_client_demo.cc     # 原始 TCP Client demo
└── demo_multi/                # 多服务编排 demo（微服务风格）
    ├── proto/demo_multi.proto # Protobuf 消息定义
    ├── conf/                  # 各服务配置
    ├── rpc_server_acl_dao_demo.cc      # ACL 权限服务
    ├── rpc_server_study_dao_demo.cc    # Study 数据服务
    ├── rpc_server_control_demo.cc      # Control 聚合服务（调用 ACL + Study）
    ├── rpc_client_demo.cc              # Protobuf 版 Client
    ├── start.sh / stop.sh              # 服务启停脚本
    └── makefile
```

---

## 3. 协议格式（Wire Protocol）

### 3.1 包结构

```
 0       4       8       12             12+N
 +-------+-------+-------+------------------+
 | Head  | Magic |FrameCode|    UserData     |
 | (4B)  | (4B)  |  (4B)  |     (NB)        |
 +-------+-------+-------+------------------+
 |<-fix->|<----------- Head.len ------------>|
```

| 字段 | 大小 | 说明 |
|------|------|------|
| **Head** | 4 字节 | 后续所有数据的总长度，即 `Magic + FrameCode + UserData` 的字节数 |
| **Magic** | 4 字节 | 固定魔数 `0xC5C5C5C5`，用于校验包合法性 |
| **FrameCode** | 4 字节 | 框架层错误码，`0` 表示正常，`[700, 799]` 表示框架错误 |
| **UserData** | N 字节 | 用户业务数据（Protobuf 序列化后的二进制内容） |

相关常量定义在 `base/common.h`：

```cpp
const int kHeadLen = 4;
const int kMagicLen = 4;
const int kFrameCodeLen = 4;
const int kExtHeadLen = kMagicLen + kFrameCodeLen;  // 8
const uint32_t kProtoMagic = 0xC5C5C5C5;
const uint32_t kFrameErrMin = 700;
const uint32_t kFrameErrMax = 799;
```

### 3.2 编解码函数

协议层通过 3 个函数指针实现可插拔设计，均定义在 `rpc_proto.h`：

| 函数指针 | 签名 | 作用 |
|----------|------|------|
| `DataProtoFunc` | `Code (*)(const char*, int, int*)` | **粘包拆包**：从 TCP 流判断一个完整包的总长度 |
| `GetUserDataFunc` | `Code (*)(const char*, int, std::string*)` | **解包**：从完整包中提取 UserData |
| `FormatUserDataFunc` | `Code (*)(const std::string, std::string*)` | **封包**：将 UserData 按协议格式封装 |

#### DefaultProtoFunc — 粘包拆包

```
读取 Head(4B) → 解码得到 len
→ 如果缓冲区数据 < Head + len → 返回 kDataNotEnough（数据不足）
→ 否则 real_len = Head + len（完整包长度）
```

#### DefaultGetUserDataFunc — 解包

```
解码 Head → 校验 Magic == 0xC5C5C5C5
→ 解码 FrameCode，如果 != 0 → 返回对应框架错误码
→ 提取 UserData = 包[12 .. 12+N]
```

#### DefaultFormatUserDataFunc — 封包

```
编码 Head = kExtHeadLen + UserData.size()
→ 编码 Magic = 0xC5C5C5C5
→ 编码 FrameCode = 0
→ 追加 UserData
```

### 3.3 框架错误响应

当服务端出现框架层错误时，`FormatFrameErrorResp()` 构造精简错误包（无 UserData）：

```
 +-------+-------+----------+
 | Head  | Magic | FrameCode|
 | (4B)  | (4B)  |   (4B)   |
 +-------+-------+----------+
  Head = kExtHeadLen = 8
```

---

## 4. 线程架构

### 4.1 三层线程模型

```
                        ┌─────────────────────────────┐
                        │        Main Thread           │
                        │    EventLoop (epoll/poll)    │
                        │    listen + accept 新连接     │
                        └──────────────┬──────────────┘
                                       │
                          accept 后 rand() 分配到某个 ConnWorker
                                       │
               ┌───────────────┬───────┴───────┬───────────────┐
               ▼               ▼               ▼               ▼
        ┌─────────────┐ ┌─────────────┐ ┌─────────────┐       ...
        │ ConnWorker-0│ │ ConnWorker-1│ │ ConnWorker-N│  (默认 10 个)
        │  EventLoop  │ │  EventLoop  │ │  EventLoop  │
        │  TCP 读写    │ │  TCP 读写    │ │  TCP 读写    │
        │  协议拆包    │ │  协议拆包    │ │  协议拆包    │
        └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
               │               │               │
               │   rand() 分配到某个 RealWorker │
               │               │               │
        ┌──────┼───────┬───────┼───────┬───────┼──────┐
        ▼      ▼       ▼      ▼       ▼      ▼       ▼
   ┌────────┐┌────────┐┌────────┐┌────────┐          ...
   │RealW-0 ││RealW-1 ││RealW-2 ││RealW-M │  (默认 50 个)
   │EventLoop││EventLoop││EventLoop││EventLoop│
   │PB 解码  ││PB 解码  ││PB 解码  ││PB 解码  │
   │业务回调 ││业务回调 ││业务回调 ││业务回调 │
   │PB 编码  ││PB 编码  ││PB 编码  ││PB 编码  │
   └────────┘└────────┘└────────┘└────────┘
```

### 4.2 各层职责

| 层级 | 类 | 线程数 | 职责 |
|------|----|--------|------|
| **Accept 层** | `RpcServer` | 1 | 监听端口，accept 新连接，分发到 ConnWorker |
| **I/O 层** | `ConnWorker` | 默认 10 | 管理客户端连接，非阻塞读写，协议粘包拆包，收发转发 |
| **业务层** | `RealWorker` | 默认 50 | Protobuf 反序列化请求，执行业务回调，序列化响应 |
| **统计线程** | `RpcServer::DumpStatAction` | 1 | 周期性 dump 统计信息 |

### 4.3 线程间通信：pipe + EventLoop

框架使用 **pipe + 加锁队列** 实现跨线程数据传递：

```
生产者线程                              消费者线程
    │                                      │
    │  1. MutexLock → 数据入队              │
    │  2. write(pipe_fd[1], "1", 1)        │
    │                                      │
    │              pipe                    │
    │  ─────────────────────────────────>  │
    │                                      │
    │                           3. EventLoop 被 pipe 可读事件唤醒
    │                           4. read(pipe_fd[0]) 清除通知
    │                           5. MutexLock → 取出队列数据
    │                           6. 处理数据
```

| 通信方向 | pipe 变量 | 数据队列 | 作用 |
|----------|-----------|----------|------|
| Main → ConnWorker | `ConnWorker::notify_fds_` | `cli_fds_` | 通知有新客户端连接到来 |
| ConnWorker → RealWorker | `RealWorker::notify_fds_` | `request_data_blocks_` | 通知有新请求需要处理 |
| RealWorker → ConnWorker | `ConnWorker::data_notify_fds_` | `resp_data_blocks_` | 通知有响应需要发送 |

---

## 5. 核心数据结构

### 5.1 TcpConn — 连接上下文

```cpp
struct TcpConn {
    std::string req_content;   // 请求读缓冲区（可能含不完整包）
    uint64_t id;               // 连接唯一标识（ConnWorker 内自增）
    int fd;                    // 客户端 socket fd
    std::string rsp_content;   // 响应写缓冲区（待发送数据）
};
```

### 5.2 OneDataBlock — 线程间传递的数据块

```cpp
struct OneDataBlock {
    std::string real_data;     // 完整的协议包数据
    uint64_t id;               // 关联的连接 id（用于响应路由回正确连接）
    int fd;                    // 关联的 socket fd
    ConnWorker *conn_worker;   // 关联的 ConnWorker 指针（用于回写响应）
};
```

### 5.3 类继承关系

```
BaseServer (抽象)
    └── RpcServer
            ├── has-many → ConnWorker (I/O 线程)
            └── has-many → RealWorker (业务线程)

BaseClient (抽象)
    └── TcpClient
            └── 被组合于 → RpcClient
```

---

## 6. 完整请求处理流程

### 6.1 Server 端启动

```
RpcServer::Init()
  ├── 读取配置（port, real_worker_num, conn_worker_num, flow_restrict 等）
  ├── 创建 TCP socket → bind → listen
  ├── 创建 Main EventLoop，注册 accept 事件
  ├── 创建 N 个 RealWorker
  │     └── 每个 RealWorker: 创建 pipe → 创建 EventLoop → pthread_create
  ├── 创建 M 个 ConnWorker
  │     └── 每个 ConnWorker: 创建 2 个 pipe → 创建 EventLoop → pthread_create
  └── 创建统计线程

RpcServer::Run()
  └── main_loop_->Run()  // 阻塞在 Main EventLoop
```

### 6.2 请求处理全链路

```
              Client                          Server
                │                               │
          ┌─────┴─────┐                         │
          │ FormatUser │                         │
          │ DataFunc() │ 封装为协议包             │
          └─────┬─────┘                         │
                │                               │
        ━━━━━━━TCP 发送━━━━━━━━━━━━━━━━━━━━━>   │
                │                               │
                │                  ┌────────────┴────────────┐
                │                  │ ① Main Thread: accept() │
                │                  │    rand() → ConnWorker-N │
                │                  └────────────┬────────────┘
                │                               │ pipe 通知
                │                  ┌────────────┴────────────┐
                │                  │ ② ConnWorker: read()     │
                │                  │    DataProtoFunc 拆包    │
                │                  │    rand() → RealWorker-M │
                │                  └────────────┬────────────┘
                │                               │ pipe 通知
                │                  ┌────────────┴────────────┐
                │                  │ ③ RealWorker:            │
                │                  │    GetUserDataFunc 提取  │
                │                  │    PB 反序列化 → req      │
                │                  │    PbRpcAction(req, resp)│
                │                  │    PB 序列化 → resp       │
                │                  │    FormatUserDataFunc 封包│
                │                  └────────────┬────────────┘
                │                               │ pipe 通知 (data_notify)
                │                  ┌────────────┴────────────┐
                │                  │ ④ ConnWorker:            │
                │                  │    匹配 (id, fd) 找到连接 │
                │                  │    Mod(fd, EV_OUT)       │
                │                  │    write() 发送响应       │
                │                  └────────────┬────────────┘
                │                               │
        <━━━━━━TCP 接收━━━━━━━━━━━━━━━━━━━━━━   │
                │                               │
          ┌─────┴─────┐                         │
          │ GetUserData│                         │
          │ Func()     │ 提取 UserData           │
          │ PB 反序列化│                         │
          └───────────┘                         │
```

### 6.3 各步骤详解

#### Step ① Main Thread — Accept 连接

```cpp
AcceptEventInternalAction(fd, evt):
  client_fd = accept(serv_fd_)
  SetFdReused(client_fd)
  SetFdNonblock(client_fd)
  n = rand() % conn_workers_.size()    // 随机选择 ConnWorker
  conn_workers_[n]->AddClientFdAndNotify(client_fd)
```

#### Step ② ConnWorker — 读取请求 & 协议拆包

```cpp
ClientEventInInternalAction(fd, evt):
  while (true):
    ret = data_proto_func_(conn.req_content)    // 检查是否收齐完整包
    if ret == kDataNotEnough:
      read(fd) → 追加到 conn.req_content        // 继续读取
      continue
    // 收到完整包
    SendRequestToRealWorker(conn.req_content, real_len, fd, id)
    conn.req_content 移除已处理部分              // 支持半包/粘包
```

#### Step ③ RealWorker — 业务处理

```cpp
DealWithRequestOneDataBlock(one_data_block):
  // 1. 提取用户数据
  get_user_data_func_(raw_data) → user_data    // 校验 Magic、FrameCode

  // 2. Protobuf 反序列化
  req_prototype_->New() → req
  req->ParseFromString(user_data)

  // 3. 执行业务回调
  action_(user_conf, req, resp)                // PbRpcAction

  // 4. Protobuf 序列化
  resp->SerializeToString() → out_data

  // 5. 按协议封装
  format_user_data_func_(out_data) → resp_data

  // 6. 回传给 ConnWorker
  conn_worker->AddResponseAndNotify(resp_data_block)
```

#### Step ④ ConnWorker — 发送响应

```cpp
DealWithRespOneDataBlock(one_data_block):
  // 通过 (id, fd) 匹配找到原始连接
  conn.rsp_content.append(resp_data)
  worker_loop_->Mod(fd, EV_IN | EV_OUT)    // 注册写事件

ClientEventOutInternalAction(fd, evt):
  while (left_len > 0):
    write(fd, rsp_content)
  // 发送完毕后
  worker_loop_->Mod(fd, EV_IN)             // 恢复为只读模式
```

---

## 7. 客户端架构

### 7.1 类结构

```
RpcClient
  ├── TcpClient tcp_client_              // 底层 TCP 收发
  ├── GetUserDataFunc get_user_data_func_    // 解包函数
  └── FormatUserDataFunc format_user_data_func_  // 封包函数
```

### 7.2 同步请求-响应流程

```cpp
RpcClient::SendAndRecv(const Message &request, Message *response):
  // 1. Protobuf 序列化
  request.SerializeToString() → serialized_request

  // 2. 调用字符串版 SendAndRecv
  //    2a. format_user_data_func_(serialized_request) → encode_request  (封包)
  //    2b. tcp_client_.SendNative(encode_request)                        (TCP 发送)
  //    2c. tcp_client_.Recv(&encode_response)                            (TCP 接收，按 DataProtoFunc 拆包)
  //    2d. get_user_data_func_(encode_response) → serialized_response   (解包)

  // 3. Protobuf 反序列化
  response->ParseFromString(serialized_response)
```

### 7.3 TcpClient 底层机制

- **非阻塞连接**：`connect()` + `epoll/poll` 等待可写，支持超时控制（`max_wait_time_ms_`）
- **非阻塞发送**：`epoll/poll` 等待 `EV_OUT`，循环 `write()` 直到全部发送
- **非阻塞接收**：`epoll/poll` 等待 `EV_IN`，`read()` 到环形缓冲区，配合 `DataProtoFunc` 拆包
- **自动重连**：发送时如果连接断开，自动调用 `ReConnect()`

---

## 8. 业务回调接口

### 8.1 PbRpcAction — Protobuf 版业务处理

```cpp
typedef Code (*PbRpcAction)(const Config &conf,
                            const ::google::protobuf::Message *req,
                            ::google::protobuf::Message *resp);
```

使用方只需实现此函数，框架负责：
- TCP 连接管理
- 协议编解码
- Protobuf 序列化/反序列化
- 线程调度

### 8.2 Prototype 模式

`RpcServer` 构造时需要传入 req/resp 的 **原型对象**（Prototype Pattern），框架通过 `prototype->New()` 创建新实例进行反序列化：

```cpp
RpcServer server(config, user_conf,
    DefaultProtoFunc, DefaultGetUserDataFunc, DefaultFormatUserDataFunc,
    MyPbRpcAction,
    &req_prototype,      // e.g. demo_multi::AclReq 实例
    &resp_prototype);    // e.g. demo_multi::AclResp 实例
```

---

## 9. 配置参数

服务端通过 `Config` 对象加载配置文件，支持以下参数：

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `port` | 9090 | 监听端口 |
| `daemon` | 0 | 是否后台运行（1 = daemon 模式 + 自动保活） |
| `real_worker_thread_num` | 50 | 业务处理线程数 |
| `conn_worker_thread_num` | 10 | I/O 连接线程数 |
| `flow_restrict` | 1000 | 流量控制上限 |
| `stat_path` | `../log/csutil_path` | 统计信息输出路径 |
| `stat_file_size` | 32MB | 统计文件大小上限 |
| `stat_dump_circle` | 60 | 统计 dump 周期（秒） |

---

## 10. demo_multi 微服务编排示例

### 10.1 服务拓扑

```
                          ┌─────────────┐
                          │   Client    │
                          │  port 9001  │
                          └──────┬──────┘
                                 │
                                 ▼
                    ┌────────────────────────┐
                    │   Control Server       │
                    │   port: 9001           │
                    │   (聚合 ACL + Study)    │
                    └─────┬──────────┬───────┘
                          │          │
                ┌─────────┘          └─────────┐
                ▼                              ▼
     ┌────────────────────┐       ┌────────────────────┐
     │   ACL Server       │       │   Study Server     │
     │   port: 9005       │       │   port: 9007       │
     │   (权限校验)        │       │   (学习数据查询)    │
     └────────────────────┘       └────────────────────┘
```

### 10.2 Protobuf 消息定义

```protobuf
// ACL 服务
message AclReq  { optional string user_name = 1; optional string resource = 2; }
message AclResp { optional BaseResp base = 1; optional bool is_allow = 2; optional string detail = 3; }

// Study 服务
message StudyReq  { optional string student_name = 1; optional string subject = 2; }
message StudyResp { optional BaseResp base = 1; optional int32 num_subject = 2; optional string detail = 3; }

// Control 聚合服务
message ControlReq  { optional string command = 1; optional string target = 2; }
message ControlResp { optional BaseResp base = 1; optional string acl_result = 2; optional string study_result = 3; }
```

### 10.3 服务间调用

Control Server 的 `ControlPbRpcAction` 中，使用 `RpcClient` 同步调用后端服务：

```cpp
// 调用 ACL 服务
RpcClient rpc_acl_client(acl_server_ip, acl_server_port);
rpc_acl_client.Init();
rpc_acl_client.SendAndRecv(acl_req, &acl_resp);

// 调用 Study 服务
RpcClient rpc_study_client(study_server_ip, study_server_port);
rpc_study_client.Init();
rpc_study_client.SendAndRecv(study_req, &study_resp);

// 聚合结果
ctrl_resp->set_acl_result(acl_resp.detail());
ctrl_resp->set_study_result(study_resp.detail());
```

### 10.4 启动方式

```bash
cd sock/demo_multi
./start.sh          # 依次启动 acl_server(9005) → study_server(9007) → control_server(9001)
./rpc_client        # 发送 Protobuf 请求到 control_server
./stop.sh           # 停止所有服务
```

---

## 11. I/O 多路复用

框架支持两种事件驱动模型，在 `RpcServer` 构造时自动选择：

```cpp
event_type_ = kPoll;
#if defined(__linux__)
event_type_ = kEPoll;      // Linux 下优先使用 epoll
#endif
```

- **EventLoop**：封装了 epoll/poll 的事件循环，每个线程独立一个 EventLoop
- **EventPoll / EventEpoll**：底层事件实现，TcpClient 中也通过 `Event` 接口使用

---

## 12. 流量控制

每个 ConnWorker 和 RealWorker 内置 `LoadCtrl` 流量控制器：

```cpp
LoadCtrl flow_ctrl_(kDefaultFlowGridNum,    // 1000 个时间格
                    kDefaultFlowUnitNum,     // 每格 10ms
                    server_->max_flow_);     // 最大流量（可配置）
```

通过滑动窗口算法限制单位时间内的请求处理量。

---

## 13. 错误处理机制

### 13.1 错误码分层

| 层级 | 范围 | 说明 |
|------|------|------|
| 框架错误 | [700, 799] | Magic 校验失败、协议解析错误等 |
| 通用错误 | 其他 Code 值 | kInvalidParam, kSocketError, kSerializePBFailed 等 |
| 业务错误 | BaseResp.ret_code | 由业务自定义，通过 Protobuf 响应体传递 |

### 13.2 框架错误传递

```
Server RealWorker 发现框架错误
  → FormatFrameErrorResp(frame_code) 构造精简错误包 [Head+Magic+FrameCode]
  → 通过 ConnWorker 发回客户端

Client 收到响应
  → DefaultGetUserDataFunc 解码 FrameCode != 0
  → 返回对应框架错误码给调用方
```

---

## 14. 关键设计总结

| 设计点 | 方案 | 说明 |
|--------|------|------|
| **线程模型** | 三层线程（Accept → IO → Biz） | 职责分离，避免业务阻塞 I/O |
| **I/O 模型** | 非阻塞 fd + epoll/poll EventLoop | 高并发连接处理 |
| **线程通信** | pipe + Mutex + deque | 低延迟跨线程唤醒 |
| **负载均衡** | rand() % N | 简单随机分配 |
| **协议设计** | 长度前缀 + Magic 校验 + FrameCode | 可靠的粘包拆包 + 框架错误透传 |
| **编解码** | 函数指针可插拔 | 允许替换自定义协议 |
| **序列化** | Protobuf + Prototype Pattern | 类型安全的消息传递 |
| **连接管理** | fd → TcpConn map + (id, fd) 路由 | 响应准确路由回原始连接 |
| **客户端** | 同步阻塞 SendAndRecv | 简单可靠，适合服务间调用 |
| **运维支持** | daemon + 统计 dump + 流量控制 | 生产可用 |
