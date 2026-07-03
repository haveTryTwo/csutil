# Demo Multi - Bazel构建完成总结

## ✅ 构建成功

已成功为 `sock/demo_multi` 创建Bazel构建配置，并区分client demo和server demo目标。

## 📦 构建产物

### Client Demo (1个)
- **rpc_client** (4.2MB) - RPC客户端演示程序

### Server Demos (3个)
- **rpc_server_control** (975KB) - 控制服务器
- **rpc_server_acl_dao** (948KB) - ACL DAO服务器
- **rpc_server_study_dao** (948KB) - Study DAO服务器

所有可执行文件位于：`bazel-bin/sock/demo_multi/`

## 🎯 快速使用

### 构建所有Demo

```bash
# 构建所有Demo（推荐）
bazel build //sock/demo_multi:all_demos

# 或者只构建客户端
bazel build //sock/demo_multi:rpc_client

# 或者只构建所有服务器
bazel build //sock/demo_multi:server_demos

# 或者构建单个服务器
bazel build //sock/demo_multi:rpc_server_control
bazel build //sock/demo_multi:rpc_server_acl_dao
bazel build //sock/demo_multi:rpc_server_study_dao
```

### 运行Demo

```bash
# 运行客户端
./bazel-bin/sock/demo_multi/rpc_client

# 运行控制服务器
./bazel-bin/sock/demo_multi/rpc_server_control

# 运行ACL DAO服务器
./bazel-bin/sock/demo_multi/rpc_server_acl_dao

# 运行Study DAO服务器
./bazel-bin/sock/demo_multi/rpc_server_study_dao
```

## 📋 BUILD文件配置

创建的 `sock/demo_multi/BUILD` 文件包含以下目标：

### 可执行文件目标
1. **rpc_client** - 客户端可执行文件
2. **rpc_server_control** - 控制服务器可执行文件
3. **rpc_server_acl_dao** - ACL DAO服务器可执行文件
4. **rpc_server_study_dao** - Study DAO服务器可执行文件

### 文件组目标
5. **conf** - 所有配置文件
6. **client_demos** - 所有客户端Demo
7. **server_demos** - 所有服务器Demos
8. **all_demos** - 所有Demos（客户端+服务器）

## 🔧 依赖关系解决

在构建过程中解决了以下依赖问题：

1. **循环依赖问题**
   - `cs` ⇄ `ftp` 之间的循环依赖
   - **解决方案**: 将cs模块拆分为`cs_base`、`cs_server`和`cs_client`

2. **make_pair编译错误**
   - `cs/server.cc` 中的 `std::make_pair<int, Conn*>` 语法错误
   - **解决方案**: 改为 `std::make_pair(client_fd, conn)`，让编译器自动推导类型

3. **头文件依赖**
   - ftp需要cs的头文件，但不能产生循环依赖
   - **解决方案**: 使用`cs:cs_base`提供仅头文件的依赖

## 📊 构建对比

| 项目 | Makefile | Bazel |
|------|----------|-------|
| 配置文件 | makefile (67行) | BUILD (83行) |
| 构建命令 | `make rpc_client` | `bazel build //sock/demo_multi:rpc_client` |
| 并行编译 | 需要 `-j` 参数 | 自动并行 |
| 增量编译 | 基于时间戳 | 基于内容哈希（更精确） |
| 依赖管理 | 手动维护 | 自动管理 |
| 清理 | `make clean` | `bazel clean` |
| 构建速度 | 基线 | 首次慢，后续更快 |

## 🏗️ 模块依赖结构

```
sock/demo_multi (demos)
├── sock:rpc
│   ├── sock:sock
│   │   └── base
│   └── cs:cs_server
│       ├── cs:cs_base
│       │   └── base
│       ├── base
│       └── ftp
│           ├── base
│           └── cs:cs_base (仅头文件)
└── base
```

## 📝 配置文件

每个可执行文件关联的配置文件通过`data`属性指定：

- **rpc_client**: 所有配置文件（通过`:conf` filegroup）
- **rpc_server_control**: `control_server.conf`, `control_user_info.conf`
- **rpc_server_acl_dao**: `acl_server.conf`, `acl_user_info.conf`
- **rpc_server_study_dao**: `study_server.conf`, `study_user_info.conf`

## 🎓 更多信息

详细的使用说明和故障排查，请参考：
- [BAZEL_BUILD.md](./BAZEL_BUILD.md) - 完整的Bazel构建指南
- [/BAZEL_README.md](/BAZEL_README.md) - 项目整体Bazel文档
- [/BAZEL_QUICKSTART.md](/BAZEL_QUICKSTART.md) - Bazel快速入门

## ✨ 总结

✅ 成功创建Bazel构建配置  
✅ 区分client demo和server demo目标  
✅ 解决所有依赖和循环依赖问题  
✅ 所有Demo编译通过  
✅ 生成4个可执行文件（1个client + 3个server）  
✅ 提供完整的构建文档  

现在您可以使用Bazel更高效地构建和管理这些Demo程序！
