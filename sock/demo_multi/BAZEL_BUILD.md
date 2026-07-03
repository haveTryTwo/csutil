# Demo Multi - Bazel构建指南

本目录包含多个RPC服务器和客户端演示程序，展示了如何使用csutil库构建分布式RPC服务。

## 目录结构

```
sock/demo_multi/
├── BUILD                           # Bazel构建配置
├── BAZEL_BUILD.md                  # 本文档
├── rpc_client_demo.cc              # RPC客户端演示代码
├── rpc_server_control_demo.cc      # 控制服务器演示代码
├── rpc_server_acl_dao_demo.cc      # ACL DAO服务器演示代码
├── rpc_server_study_dao_demo.cc    # Study DAO服务器演示代码
└── conf/                           # 配置文件目录
    ├── control_server.conf         # 控制服务器配置
    ├── control_user_info.conf      # 控制服务器用户信息
    ├── acl_server.conf             # ACL服务器配置
    ├── acl_user_info.conf          # ACL服务器用户信息
    ├── study_server.conf           # Study服务器配置
    └── study_user_info.conf        # Study服务器用户信息
```

## 构建目标

### 1. 客户端Demo

编译RPC客户端：

```bash
bazel build //sock/demo_multi:rpc_client
```

生成的可执行文件：
- `bazel-bin/sock/demo_multi/rpc_client`

运行客户端：

```bash
./bazel-bin/sock/demo_multi/rpc_client
```

### 2. 服务器Demos

#### 2.1 控制服务器

编译控制服务器：

```bash
bazel build //sock/demo_multi:rpc_server_control
```

生成的可执行文件：
- `bazel-bin/sock/demo_multi/rpc_server_control`

配置文件：
- `conf/control_server.conf`
- `conf/control_user_info.conf`

#### 2.2 ACL DAO服务器

编译ACL DAO服务器：

```bash
bazel build //sock/demo_multi:rpc_server_acl_dao
```

生成的可执行文件：
- `bazel-bin/sock/demo_multi/rpc_server_acl_dao`

配置文件：
- `conf/acl_server.conf`
- `conf/acl_user_info.conf`

#### 2.3 Study DAO服务器

编译Study DAO服务器：

```bash
bazel build //sock/demo_multi:rpc_server_study_dao
```

生成的可执行文件：
- `bazel-bin/sock/demo_multi/rpc_server_study_dao`

配置文件：
- `conf/study_server.conf`
- `conf/study_user_info.conf`

### 3. 批量构建

#### 构建所有客户端Demo

```bash
bazel build //sock/demo_multi:client_demos
```

#### 构建所有服务器Demos

```bash
bazel build //sock/demo_multi:server_demos
```

#### 构建所有Demos

```bash
bazel build //sock/demo_multi:all_demos
```

或者使用通配符：

```bash
bazel build //sock/demo_multi:*
```

## 编译选项

### Debug模式编译

```bash
bazel build //sock/demo_multi:rpc_client --copt=-g --compilation_mode=dbg
```

### Release模式编译

```bash
bazel build //sock/demo_multi:rpc_client --compilation_mode=opt
```

### 带调试符号的优化编译

```bash
bazel build //sock/demo_multi:rpc_client --copt=-g --compilation_mode=opt
```

## 运行说明

### 启动服务器

按以下顺序启动服务器：

1. 启动ACL DAO服务器：
```bash
./bazel-bin/sock/demo_multi/rpc_server_acl_dao
```

2. 启动Study DAO服务器：
```bash
./bazel-bin/sock/demo_multi/rpc_server_study_dao
```

3. 启动控制服务器：
```bash
./bazel-bin/sock/demo_multi/rpc_server_control
```

### 运行客户端

所有服务器启动后，运行客户端：

```bash
./bazel-bin/sock/demo_multi/rpc_client
```

## 依赖关系

这些Demo依赖以下库：

- `//base` - 基础工具库
- `//sock:sock` - TCP/Socket基础库
- `//sock:rpc` - RPC框架库

## 与Makefile的对比

| 功能 | Makefile | Bazel |
|------|----------|-------|
| 构建客户端 | `make rpc_client` | `bazel build //sock/demo_multi:rpc_client` |
| 构建所有服务器 | `make rpc_server_control rpc_server_acl_dao rpc_server_study_dao` | `bazel build //sock/demo_multi:server_demos` |
| 构建所有 | `make all` | `bazel build //sock/demo_multi:all_demos` |
| 清理 | `make clean` | `bazel clean` |
| 增量编译 | ✓ | ✓✓ (更智能) |
| 并行编译 | 手动指定 `-j` | 自动并行 |
| 依赖管理 | 手动维护 | 自动管理 |

## 配置文件路径

Bazel编译后，配置文件会被包含在`data`属性中。运行时需要确保配置文件在正确的位置。

建议的运行方式：

```bash
# 方式1：在项目根目录运行
cd /path/to/csutil
./bazel-bin/sock/demo_multi/rpc_server_control

# 方式2：在demo_multi目录运行
cd sock/demo_multi
../../bazel-bin/sock/demo_multi/rpc_server_control
```

## 故障排查

### 编译错误

如果遇到编译错误，尝试清理并重新编译：

```bash
bazel clean
bazel build //sock/demo_multi:all_demos
```

### 找不到配置文件

确保配置文件在`conf/`目录下，或者修改代码中的配置文件路径。

### 端口占用

如果提示端口已被占用，检查是否有其他进程在使用相同端口，或修改配置文件中的端口号。

## 更多信息

- 主项目Bazel文档: `/BAZEL_README.md`
- Bazel快速入门: `/BAZEL_QUICKSTART.md`
- 构建对比: `/BUILD_COMPARISON.md`
